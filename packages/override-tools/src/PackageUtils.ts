/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';

/**
 * Try to find the currently installed version of React Native by seaching for
 * it's package.json.
 */
export async function getInstalledRNVersion(
  targetPath: string,
): Promise<string> {
  const relativeRnPackage = 'node_modules\\react-native\\package.json';

  let searchPath = path.resolve(targetPath);
  while (path.resolve(searchPath, '..') !== searchPath) {
    try {
      const rnPackage = path.join(searchPath, relativeRnPackage);
      const packageJson = (await fs.promises.readFile(rnPackage)).toString();
      const version = JSON.parse(packageJson).version;

      if (typeof version !== 'string') {
        throw new Error('Unexpected formt of React Native package.json');
      }

      return version;
    } catch (ex) {
      if (ex.code === 'ENOENT') {
        searchPath = path.resolve(searchPath, '..');
      } else {
        throw ex;
      }
    }
  }

  throw new Error('Could not find a valid package.json for React Native');
}
