/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * GitHub operations via the gh CLI.
 *
 * @format
 */

import {exec} from './proc';

export interface PRInfo {
  number: number;
  url: string;
}

/**
 * Find an existing open PR by head branch name.
 * Returns null if no matching PR exists.
 */
export async function findPR(opts: {
  head: string;
  cwd: string;
}): Promise<PRInfo | null> {
  const result = await exec(
    `gh pr list --head "${opts.head}" --json number,url --limit 1`,
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
 */
export async function createPR(opts: {
  head: string;
  base: string;
  title: string;
  body: string;
  cwd: string;
}): Promise<PRInfo> {
  const result = await exec(
    `gh pr create --head "${opts.head}" --base "${opts.base}"` +
      ` --title "${escapeForShell(opts.title)}"` +
      ` --body "${escapeForShell(opts.body)}"`,
    {cwd: opts.cwd},
  );

  // gh pr create outputs the PR URL on success
  const url = result.trim();
  const match = url.match(/\/pull\/(\d+)/);
  const number = match ? parseInt(match[1], 10) : 0;

  return {number, url};
}

/**
 * Update an existing pull request's body text.
 */
export async function updatePR(opts: {
  number: number;
  body: string;
  cwd: string;
}): Promise<void> {
  await exec(
    `gh pr edit ${opts.number} --body "${escapeForShell(opts.body)}"`,
    {cwd: opts.cwd},
  );
}

/**
 * Escape a string for safe inclusion in a double-quoted shell argument.
 */
function escapeForShell(str: string): string {
  return str
    .replace(/\\/g, '\\\\')
    .replace(/"/g, '\\"')
    .replace(/\$/g, '\\$')
    .replace(/`/g, '\\`');
}
