/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import * as minimatch from 'minimatch';
import * as path from 'path';

import {OverrideFileRepository, ReactFileRepository} from '../FileRepository';

export interface MockFile {
  filename: string;
  content: string | Buffer;
}

export class MockReactFileRepository implements ReactFileRepository {
  private files: Array<MockFile>;

  constructor(files: Array<MockFile>) {
    this.files = files;
    this.files.forEach(file => (file.filename = path.normalize(file.filename)));
  }

  async getFileContents(filename: string): Promise<Buffer | null> {
    const matches = _.filter(this.files, file => file.filename === filename);
    if (matches.length === 0) {
      return null;
    } else {
      return Buffer.from(matches[0].content);
    }
  }

  getVersion(): string {
    return '0.61.5';
  }
}

export class MockOverrideFileRepository implements OverrideFileRepository {
  private files: Array<MockFile>;

  constructor(files: Array<MockFile>) {
    this.files = files;
    this.files.forEach(file => (file.filename = path.normalize(file.filename)));
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

  async getFileContents(filename: string): Promise<Buffer | null> {
    const matches = this.files.filter(file => file.filename === filename);
    if (matches.length === 0) {
      return null;
    } else {
      return Buffer.from(matches[0].content);
    }
  }

  async setFileContents(filename: string, content: Buffer) {
    const matchFile = this.files.find(file => file.filename === filename)!;
    matchFile.content = content;
  }
}
