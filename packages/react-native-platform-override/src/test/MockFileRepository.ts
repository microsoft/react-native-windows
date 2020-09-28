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

import {normalizePath} from '../PathUtils';

export interface MockFile {
  filename: string;
  content: string | Buffer;
}

export interface MockEmptyDirectory {
  dirname: string;
}

export default class MockFileRepository implements FileRepository {
  protected files: MockFile[];
  protected directories: MockEmptyDirectory[];

  constructor(files: MockFile[], directories?: MockEmptyDirectory[]) {
    this.files = files.map(file => ({
      ...file,
      filename: normalizePath(file.filename),
    }));

    this.directories = (directories || []).map(dir => ({
      dirname: normalizePath(dir.dirname),
    }));
  }

  async listFiles(globs: string[] = ['**']): Promise<string[]> {
    const parsedGlobs = globs.map(g => new minimatch.Minimatch(g, {dot: true}));
    const includeGlobs = parsedGlobs.filter(m => !m.negate);
    const excludeGlobs = parsedGlobs.filter(m => m.negate);

    return this.files
      .map(file => file.filename)
      .filter(file => includeGlobs.some(g => g.match(file)))
      .filter(file => excludeGlobs.every(g => g.match(file)));
  }

  async readFile(filename: string): Promise<Buffer | null> {
    const normalizedName = normalizePath(filename);
    const file = this.files.find(f => f.filename === normalizedName);
    return file ? Buffer.from(file.content) : null;
  }

  async stat(filename: string): Promise<'file' | 'directory' | 'none'> {
    const normalizedName = normalizePath(filename);

    if (this.directories.find(dir => dir.dirname === normalizedName)) {
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
