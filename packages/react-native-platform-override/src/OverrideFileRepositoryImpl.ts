/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as globby from 'globby';
import * as path from 'path';

import {OverrideFileRepository} from './FileRepository';

/**
 * Allows reading phsyical override files based on a passed in directory
 */
export default class OverrideFileRepositoryImpl
  implements OverrideFileRepository {
  private baseDir: string;

  constructor(baseDir: string) {
    this.baseDir = baseDir;
  }

  async listFiles(globs: string[] = ['**']): Promise<string[]> {
    return await globby(globs, {
      cwd: this.baseDir,
      dot: true,
      ignore: ['./overrides.json'],
    });
  }

  async getFileContents(filename: string): Promise<Buffer | null> {
    const filePath = path.join(this.baseDir, filename);
    try {
      return await fs.promises.readFile(filePath);
    } catch {
      return null;
    }
  }

  async setFileContents(filename: string, content: Buffer) {
    const filePath = path.join(this.baseDir, filename);
    return fs.promises.writeFile(filePath, content);
  }
}
