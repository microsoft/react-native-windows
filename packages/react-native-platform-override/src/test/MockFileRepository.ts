/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as minimatch from 'minimatch';
import * as path from 'path';

import FileRepository, {
  ReactFileRepository,
  WritableFileRepository,
} from '../FileRepository';

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
      filename: path.normalize(file.filename),
    }));

    this.directories = (directories || []).map(dir => ({
      dirname: path.normalize(dir.dirname),
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
    const normalizedName = path.normalize(filename);
    const file = this.files.find(f => f.filename === normalizedName);
    return file ? Buffer.from(file.content) : null;
  }

  async stat(filename: string): Promise<'file' | 'directory' | 'none'> {
    const normalizedName = path.normalize(filename);

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
    const matchFile = this.files.find(file => file.filename === filename)!;
    matchFile.content = content;
  }
}
