/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';

import {ReactFileRepository} from './FileRepository';
import {getInstalledRNVersion} from './PackageUtils';

const rnPath = path.dirname(require.resolve('react-native/package.json'));

/**
 * ReactFileRepository pointing to the locally installed package for
 * react-native.
 */
export class PackageReactFileRepository implements ReactFileRepository {
  async getFileContents(filename: string): Promise<string | null> {
    const fullPath = path.join(rnPath, filename);
    try {
      return (await fs.promises.readFile(fullPath)).toString();
    } catch {
      return null;
    }
  }

  getVersion(): string {
    return getInstalledRNVersion();
  }
}
