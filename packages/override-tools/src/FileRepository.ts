/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Provides access to patch files
 */
export interface OverrideFileRepository {
  /**
   * Return the repository-relative path to all patch files
   */
  listFiles(): Promise<Array<string>>;

  /**
   * Read the contents of a patch file
   */
  getFileContents(filename: string): Promise<string | null>;
}

/**
 * Provides access to React Native source files
 */
export interface ReactFileRepository {
  getFileContents(filename: string): Promise<string | null>;
}

/**
 * Provides access to React Native source files of arbitrary version
 */
export interface VersionedReactFileRepository {
  getFileContents(
    filename: string,
    reactNativeVersion: string,
  ): Promise<string | null>;
}

/**
 * Convert from a VersionedReactFileRepository to ReactFileRepository
 */
export function bindVersion(
  repository: VersionedReactFileRepository,
  version: string,
): ReactFileRepository {
  return {
    getFileContents: (filename: string) =>
      repository.getFileContents(filename, version),
  };
}
