/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as globby from 'globby';
import * as path from 'path';

import {WritableFileRepository} from './FileRepository';

/**
 * Allows reading phsyical files based on a passed in directory
 */
export default class FileSystemRepository implements WritableFileRepository {
  private readonly baseDir: string;

  constructor(baseDir: string) {
    this.baseDir = baseDir;
  }

  async listFiles(globs: string[] = ['**']): Promise<string[]> {
    return await globby(globs, {
      cwd: this.baseDir,
      dot: true,
    });
  }

  async readFile(filename: string): Promise<Buffer | null> {
    const filePath = path.join(this.baseDir, filename);
    try {
      return await fs.promises.readFile(filePath);
    } catch {
      return null;
    }
  }

  async stat(filename: string): Promise<'file' | 'directory' | 'none'> {
    const filePath = path.join(this.baseDir, filename);
    try {
      const stats = await fs.promises.stat(filePath);
      return stats.isDirectory() ? 'directory' : 'file';
    } catch (ex) {
      if (ex.code === 'ENOENT') {
        return 'none';
      } else {
        throw ex;
      }
    }
  }

  async writeFile(filename: string, content: Buffer | string) {
    const filePath = path.join(this.baseDir, filename);
    await fs.promises.mkdir(path.dirname(filePath), {recursive: true});
    return fs.promises.writeFile(filePath, content);
  }

  async deleteFile(filename: string): Promise<void> {
    const filePath = path.join(this.baseDir, filename);
    await fs.promises.unlink(filePath);
  }
}
