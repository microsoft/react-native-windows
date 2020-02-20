/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from './fs-promise';
import * as path from 'path';

import {IOverrideFileRepository} from './FileRepositoryInterface';

/**
 * Allows reading phsyical override files based on a passed in directory
 */
export class OverrideFileRepository implements IOverrideFileRepository {
  private baseDir: string;

  constructor(baseDir: string) {
    this.baseDir = baseDir;
  }

  async listFiles(): Promise<string[]> {
    return this.listFilesRec(this.baseDir);
  }

  async getFileContents(filename: string): Promise<string | null> {
    const filePath = path.join(this.baseDir, filename);
    if (await fs.exists(filePath)) {
      return (await fs.readFile(filename)).toString();
    } else {
      return null;
    }
  }

  private async listFilesRec(file: string): Promise<Array<string>> {
    const stat = await fs.stat(file);
    if (!stat.isDirectory()) {
      return [file];
    }

    const ret: Array<string> = [];
    const subfiles = await fs.readdir(file);

    await Promise.all(
      subfiles.map(async subfile => {
        const fullPath = path.join(file, subfile);
        const childFiles = await this.listFilesRec(fullPath);
        ret.push(...childFiles);
      }),
    );

    return ret;
  }
}
