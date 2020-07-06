/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

import * as findUp from 'find-up';
import * as fs from 'fs';
import * as path from 'path';

/**
 * Find the root directory of the repo upward from cwd
 */
export default async (): Promise<string> => {
  const root = await findUp(
    async (dir: string): Promise<findUp.Match> => {
      const packagePath = path.join(dir, 'package.json');
      if (!(await findUp.exists(packagePath))) {
        return undefined;
      }

      const pkg = (await fs.promises.readFile(packagePath)).toString();
      return JSON.parse(pkg).name === 'react-native-windows-repo'
        ? dir
        : undefined;
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
