/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

import * as findUp from 'find-up';
import * as path from 'path';

/**
 * Find the root directory of a repo upward from cwd
 */
export default async (): Promise<string> => {
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
};
