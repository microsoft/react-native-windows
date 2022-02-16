/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

import findUp from 'find-up';
import fs from '@react-native-windows/fs';
import path from 'path';

/**
 * Find the root directory of a repo upward from cwd
 */
async function findRepoRoot(): Promise<string> {
  const root = await findUp(
    async (dir: string): Promise<findUp.Match> => {
      return (await findUp.exists(path.join(dir, '.git'))) ? dir : undefined;
    },
    {type: 'directory'},
  );

  if (!root) {
    throw new Error(
      'Unable to find the root of react-native-windows. Are you running within the repo?',
    );
  }

  return root;
}

/**
 * Synchronously finds the root directory of a repo upward from cwd
 */
function findRepoRootSync(): string {
  const root = findUp.sync(
    (dir: string): findUp.Match => {
      return fs.existsSync(path.join(dir, '.git')) ? dir : undefined;
    },
    {type: 'directory'},
  );

  if (!root) {
    throw new Error(
      'Unable to find the root of react-native-windows. Are you running within the repo?',
    );
  }

  return root;
}

const exportObj = Object.assign(findRepoRoot, {sync: findRepoRootSync});
export default exportObj;
