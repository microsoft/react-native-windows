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
   * Read the contents of a patch file.
   * @param filename is expected to be relative to the React Native source root.
   */
  getFileContents(filename: string): Promise<string | null>;

  /**
   * Sets the contents of an override file. Rejects the promise if the override
   * doesn't exist.
   */
  setFileContents(filename: string, content: string): Promise<void>;
}

/**
 * Provides access to React Native source files
 */
export interface ReactFileRepository {
  getFileContents(filename: string): Promise<string | null>;
  getVersion(): string;
}

/**
 * Provides access to React Native source files of arbitrary version
 */
export interface VersionedReactFileRepository {
  getFileContents(filename: string, version: string): Promise<string | null>;
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
    getVersion: () => version,
  };
}
