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
   * Return the repository-relative path to all override files
   *
   * @param globs optional list of globs which files must match
   */
  listFiles(globs?: string[]): Promise<Array<string>>;

  /**
   * Read the contents of a patch file.
   * @param filename is expected to be relative to the repository root.
   */
  getFileContents(filename: string): Promise<Buffer | null>;

  /**
   * Sets the contents of an override file. Rejects the promise if the override
   * doesn't exist.
   */
  setFileContents(filename: string, content: Buffer): Promise<void>;
}

/**
 * Provides access to React Native source files
 */
export interface ReactFileRepository {
  /**
   * Read the contents of a source file.
   * @param filename is expected to be relative to the React Native source root.
   */
  getFileContents(filename: string): Promise<Buffer | null>;

  /**
   * Get the React Native version the repo is exploring
   */
  getVersion(): string;
}

/**
 * Provides access to React Native source files of arbitrary version
 * {@see ReactFileRepository} for more details
 */
export interface VersionedReactFileRepository {
  getFileContents(filename: string, version: string): Promise<Buffer | null>;
}

/**
 * Convert from a VersionedReactFileRepository to ReactFileRepository
 */
export function bindVersion(
  repository: VersionedReactFileRepository,
  version: string,
): ReactFileRepository {
  return {
    getFileContents: filename => repository.getFileContents(filename, version),
    getVersion: () => version,
  };
}
