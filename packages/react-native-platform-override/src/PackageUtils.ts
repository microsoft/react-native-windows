/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import {findThisPackage} from './FileSearch';

let npmPackage: any = null;

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
 * Return an object representing the package.json of our current package
 */
export async function getNpmPackage(): Promise<any> {
  if (npmPackage !== null) {
    return npmPackage;
  }

  const npmPackagePath = await findThisPackage();
  const npmPackageContent = await fs.promises.readFile(npmPackagePath);
  npmPackage = JSON.parse(npmPackageContent.toString());

  return npmPackage;
}
