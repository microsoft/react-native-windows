/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as path from 'path';
import * as upath from 'upath';

/**
 * Convert a path to use Unix-style slashes
 */
export function unixPath(filepath: string): string {
  return upath.toUnix(filepath);
}

/**
 * Normalize a path and convert to use platform-specific slashes
 */
export function normalizePath(filepath: string): string {
  // path.normalize will convert unix paths to win32, but not the other way
  // around. Normalize to Unix paths first to get both cases.
  const unixStylePath = unixPath(filepath);
  return path.normalize(unixStylePath);
}
