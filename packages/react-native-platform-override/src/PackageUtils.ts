/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as path from 'path';

/**
 * Try to find the currently installed React Native version by searching for and
 * reading it's package.json.
 */
export async function getInstalledRNVersion(): Promise<string> {
  const rnPackage = require('react-native/package.json');
  const version = rnPackage.version;

  if (typeof version !== 'string') {
    throw new Error('Unexpected formt of React Native package.json');
  }

  return version;
}

/**
 * Return an object representing the package.json of this package
 */
export function getNpmPackage(): any {
  const npmPackageDir = path.join(
    path.dirname(require.main.filename),
    'package.json',
  );

  return require(npmPackageDir);
}
