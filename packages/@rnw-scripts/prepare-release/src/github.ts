/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * GitHub operations via the gh CLI.
 *
 * @format
 */

import fs from 'fs';
import os from 'os';
import path from 'path';

import {exec} from './proc';

export interface PRInfo {
  number: number;
  url: string;
}

/**
 * Run a callback with a temporary file containing the given content.
 * The file is cleaned up after the callback completes (success or failure).
 */
async function withTempFile<T>(
  content: string,
  fn: (filePath: string) => Promise<T>,
): Promise<T> {
  const tmpFile = path.join(
    os.tmpdir(),
    `gh-pr-body-${process.pid}-${Date.now()}.md`,
  );
  fs.writeFileSync(tmpFile, content, 'utf8');
  try {
    return await fn(tmpFile);
  } finally {
    try {
      fs.unlinkSync(tmpFile);
    } catch {
      // Ignore cleanup errors
    }
  }
}

/**
 * Find an existing open PR by head branch name.
 * Returns null if no matching PR exists.
 */
export async function findPR(opts: {
  head: string;
  cwd: string;
  repo?: string;
}): Promise<PRInfo | null> {
  const repoFlag = opts.repo ? ` --repo "${opts.repo}"` : '';
  const result = await exec(
    `gh pr list --head "${opts.head}" --json number,url --limit 1${repoFlag}`,
    {cwd: opts.cwd, fallback: '[]'},
  );

  let prs: Array<{number: number; url: string}>;
  try {
    prs = JSON.parse(result);
  } catch {
    return null;
  }

  if (!Array.isArray(prs) || prs.length === 0) {
    return null;
  }

  return {number: prs[0].number, url: prs[0].url};
}

/**
 * Create a new pull request. Returns the PR number and URL.
 *
 * Uses --body-file with a temp file to avoid shell escaping issues
 * with multiline markdown content on Windows cmd.exe.
 */
export async function createPR(opts: {
  head: string;
  base: string;
  title: string;
  body: string;
  cwd: string;
  repo?: string;
}): Promise<PRInfo> {
  return withTempFile(opts.body, async bodyFile => {
    const repoFlag = opts.repo ? ` --repo "${opts.repo}"` : '';
    const result = await exec(
      `gh pr create --head "${opts.head}" --base "${opts.base}"` +
        ` --title "${escapeForShell(opts.title)}"` +
        ` --body-file "${bodyFile}"${repoFlag}`,
      {cwd: opts.cwd},
    );

    // gh pr create outputs the PR URL on success
    const url = result.trim();
    const match = url.match(/\/pull\/(\d+)/);
    const number = match ? parseInt(match[1], 10) : 0;

    return {number, url};
  });
}

/**
 * Update an existing pull request's body text.
 *
 * Uses --body-file with a temp file to avoid shell escaping issues
 * with multiline markdown content on Windows cmd.exe.
 */
export async function updatePR(opts: {
  number: number;
  body: string;
  cwd: string;
  repo?: string;
}): Promise<void> {
  await withTempFile(opts.body, async bodyFile => {
    const repoFlag = opts.repo ? ` --repo "${opts.repo}"` : '';
    await exec(
      `gh pr edit ${opts.number} --body-file "${bodyFile}"${repoFlag}`,
      {cwd: opts.cwd},
    );
  });
}

/**
 * Escape a string for safe inclusion in a double-quoted shell argument.
 * Used for single-line values like titles; multiline content uses --body-file.
 */
function escapeForShell(str: string): string {
  return str
    .replace(/\\/g, '\\\\')
    .replace(/"/g, '\\"')
    .replace(/\$/g, '\\$')
    .replace(/`/g, '\\`');
}
