/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';

/**
 * Search for a manifest adjacent to the package above cwd
 */
export async function findManifest(cwd?: string): Promise<string> {
  const packagePath = await findFileAbove(cwd || process.cwd(), 'package.json');
  if (!packagePath) {
    throw new Error('This command must be run within a package');
  }

  const manifestPath = path.join(path.dirname(packagePath), 'overrides.json');
  if (!(await exists(manifestPath))) {
    throw new Error(
      'Expected an "overrides.json" file at the root of the current package',
    );
  }

  return manifestPath;
}

/**
 * Seatch for the package.json for this package (react-native-platform-override)
 */
export async function findThisPackage(): Promise<string> {
  const thisPackagePath = await findFileAbove(__dirname, 'package.json');
  if (!thisPackagePath) {
    throw new Error(
      'Unable to find a package.json about our source code. Did directory structure change?',
    );
  }

  return thisPackagePath;
}

/**
 * Search for the target path segment in all directories above the base
 */
async function findFileAbove(
  base: string,
  target: string,
): Promise<string | null> {
  const searchPath = path.resolve(base);
  const fullPath = path.join(searchPath, target);
  if (await exists(fullPath)) {
    return fullPath;
  }

  const searchPathParent = path.resolve(path.join(searchPath, '..'));
  if (searchPathParent === searchPath) {
    return null;
  } else {
    return findFileAbove(searchPathParent, target);
  }
}

/**
 * Asyncrhonusly tests if a file exists
 */
async function exists(filepath: string): Promise<boolean> {
  try {
    await fs.promises.access(filepath);
    return true;
  } catch (ex) {
    if (ex.code === 'ENOENT') {
      return false;
    } else {
      throw ex;
    }
  }
}
