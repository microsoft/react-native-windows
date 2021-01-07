/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as minimatch from 'minimatch';

import FileRepository, {
  ReactFileRepository,
  WritableFileRepository,
} from '../FileRepository';

import {normalizePath, unixPath} from '../PathUtils';

export interface MockFile {
  filename: string;
  content: string | Buffer;
}

export default class MockFileRepository implements FileRepository {
  protected files: MockFile[];
  protected emptyDirectories: string[];
  private readonly normalize: (file: string) => string;

  constructor(
    files: MockFile[],
    opts?: {
      emptyDirectories?: string[];
      rawPaths?: boolean;
    },
  ) {
    const emptyDirectories = (opts && opts.emptyDirectories) || [];
    this.normalize =
      opts && opts.rawPaths ? (file: string) => file : normalizePath;

    this.files = files.map(file => ({
      ...file,
      filename: this.normalize(file.filename),
    }));

    this.emptyDirectories = emptyDirectories.map(this.normalize);
  }

  async listFiles(globs: string[] = ['**']): Promise<string[]> {
    const parsedGlobs = globs.map(g => new minimatch.Minimatch(g, {dot: true}));
    const includeGlobs = parsedGlobs.filter(m => !m.negate);
    const excludeGlobs = parsedGlobs.filter(m => m.negate);

    return this.files
      .map(file => file.filename)
      .filter(file => includeGlobs.some(g => g.match(unixPath(file))))
      .filter(file => excludeGlobs.every(g => g.match(unixPath(file))));
  }

  async readFile(filename: string): Promise<Buffer | null> {
    const normalizedName = this.normalize(filename);
    const file = this.files.find(f => f.filename === normalizedName);
    return file ? Buffer.from(file.content) : null;
  }

  async stat(filename: string): Promise<'file' | 'directory' | 'none'> {
    const normalizedName = this.normalize(filename);

    if (this.emptyDirectories.find(dir => dir === normalizedName)) {
      return 'directory';
    }

    for (const file of this.files) {
      if (file.filename === normalizedName) {
        return 'file';
      } else if (file.filename.startsWith(normalizedName)) {
        return 'directory';
      }
    }

    return 'none';
  }
}

export class MockReactFileRepository extends MockFileRepository
  implements ReactFileRepository {
  getVersion(): string {
    return '0.61.5';
  }
}

export class MockWritableFileRepository extends MockFileRepository
  implements WritableFileRepository {
  async writeFile(filename: string, content: Buffer) {
    const matchFile = this.files.find(file => file.filename === filename);
    if (matchFile) {
      matchFile.content = content;
    } else {
      this.files.push({filename, content});
    }
  }

  async deleteFile(filename: string): Promise<void> {
    const matchIdx = this.files.findIndex(file => file.filename === filename);

    if (matchIdx === -1) {
      const err = new Error(`Mock file ${filename} not found`);
      (err as NodeJS.ErrnoException).code = 'ENOENT';
      throw err;
    }

    this.files.splice(matchIdx);
  }
}
