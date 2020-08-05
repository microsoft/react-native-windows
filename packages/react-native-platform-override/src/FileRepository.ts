/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * A filesystem abstraction that allows listing and reading files
 */
export default interface FileRepository {
  /**
   * Return the repository-relative path to files
   * @param globs optional list of globs which files must match
   */
  listFiles(globs?: string[]): Promise<Array<string>>;

  /**
   * Read the contents of a patch file.
   * @param filename is expected to be relative to the repository root.
   */
  readFile(filename: string): Promise<Buffer | null>;

  /**
   * Check whether a file exists, and if so whether its a file or directory
   */
  stat(filename: string): Promise<'file' | 'directory' | 'none'>;
}

/**
 * Provides access to files
 */
export interface WritableFileRepository extends FileRepository {
  /**
   * Sets the contents of a file. Rejects the promise if the file doesn't
   * exist.
   */
  writeFile(filename: string, content: Buffer | string): Promise<void>;

  /**
   * Deletes the file/directory, rejecting the promise if it doesn't exist.
   */
  deleteFile(filename: string): Promise<void>;
}

/**
 * Provides access to React Native source files
 */
export interface ReactFileRepository extends FileRepository {
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
  listFiles(
    globs: string[] | undefined,
    version: string,
  ): Promise<Array<string>>;
  readFile(filename: string, version: string): Promise<Buffer | null>;
  stat(
    filename: string,
    version: string,
  ): Promise<'file' | 'directory' | 'none'>;
}

/**
 * Convert from a VersionedReactFileRepository to ReactFileRepository
 */
export function bindVersion(
  repository: VersionedReactFileRepository,
  version: string,
): ReactFileRepository {
  return {
    listFiles: globs => repository.listFiles(globs, version),
    readFile: filename => repository.readFile(filename, version),
    stat: filename => repository.stat(filename, version),
    getVersion: () => version,
  };
}
