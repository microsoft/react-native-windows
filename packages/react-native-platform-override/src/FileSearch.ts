/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';

/**
 * Search for a manifest above an override file and return its path
 */
export async function findManifest(overridePath: string): Promise<string> {
  return findFileAbove(overridePath, 'overrides.json');
}

/**
 * Search for the target path segment in all directories above the base
 */
async function findFileAbove(base: string, target: string): Promise<string> {
  let searchPath = path.resolve(base);
  while (path.resolve(searchPath, '..') !== searchPath) {
    try {
      const fullPath = path.join(searchPath, target);
      await fs.promises.access(fullPath);
      return fullPath;
    } catch (ex) {
      if (ex.code === 'ENOENT') {
        searchPath = path.resolve(searchPath, '..');
      } else {
        throw ex;
      }
    }
  }

  throw new Error(`Could not find file '${target}'`);
}
