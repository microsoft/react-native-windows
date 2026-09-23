/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Beachball change file detection and version bump invocation.
 *
 * @format
 */

import fs from '@react-native-windows/fs';
import path from 'path';

import {exec} from './proc';

/**
 * Check whether there are pending beachball change files in the repo.
 *
 * Beachball stores change files as JSON in the `change/` directory at the
 * repo root. If there are any .json files there, there are pending changes.
 */
export function hasChangeFiles(repoRoot: string): boolean {
  const changeDir = path.join(repoRoot, 'change');
  if (!fs.existsSync(changeDir)) {
    return false;
  }

  const entries = fs.readdirSync(changeDir);
  return entries.some(entry => entry.endsWith('.json'));
}

/**
 * Run beachball bump to consume change files and update versions/changelogs.
 *
 * Invokes: npx beachball bump --branch <remote>/<target> --yes --verbose
 *
 * The --yes flag suppresses prompts.
 * The --verbose flag provides detailed output.
 * The --branch flag tells beachball the baseline branch to diff against.
 */
export async function bumpVersions(opts: {
  targetBranch: string;
  remote: string;
  cwd: string;
}): Promise<void> {
  await exec(
    `npx beachball bump --branch ${opts.remote}/${opts.targetBranch} --yes --verbose`,
    {cwd: opts.cwd},
  );
}

/**
 * Regenerate yarn.lock so it matches the versions beachball just bumped.
 *
 * beachball rewrites package.json/changelogs but never touches the lockfile, so
 * without this the lockfile drifts and every downstream `yarn install
 * --immutable` fails with YN0028. `--mode=update-lockfile` refreshes the
 * lockfile without touching node_modules; immutable installs are on by default
 * under CI, so disable that for this one call.
 */
export async function updateLockfile(opts: {cwd: string}): Promise<void> {
  await exec('yarn install --mode=update-lockfile', {
    cwd: opts.cwd,
    env: {YARN_ENABLE_IMMUTABLE_INSTALLS: 'false'},
  });
}
