/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import path from 'path';
import upath from 'upath';
import {minimatch} from 'minimatch';

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

/**
 * Match exclusions relative to a copied directory, including dotfiles.
 */
export function isFileExcluded(
  filename: string,
  directory: string,
  excludePatterns: readonly string[] = [],
): boolean {
  const relativePath = unixPath(
    path.relative(normalizePath(directory), normalizePath(filename)),
  );
  return excludePatterns.some(pattern =>
    minimatch(relativePath, unixPath(pattern), {
      dot: true,
      nonegate: true,
      nocomment: true,
    }),
  );
}
