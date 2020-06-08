/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import * as path from 'path';

import {OverrideFileRepository, ReactFileRepository} from '../FileRepository';

export interface MockFile {
  filename: string;
  content: string;
}

export class MockReactFileRepository implements ReactFileRepository {
  private files: Array<MockFile>;

  constructor(files: Array<MockFile>) {
    this.files = files;
    this.files.forEach(file => (file.filename = path.normalize(file.filename)));
  }

  async getFileContents(filename: string): Promise<string | null> {
    const matches = _.filter(this.files, file => file.filename === filename);
    if (matches.length === 0) {
      return null;
    } else {
      return matches[0].content;
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

  async listFiles(): Promise<string[]> {
    return this.files.map(file => file.filename);
  }

  async getFileContents(filename: string): Promise<string | null> {
    const matches = this.files.filter(file => file.filename === filename);
    if (matches.length === 0) {
      return null;
    } else {
      return matches[0].content;
    }
  }

  async setFileContents(filename: string, content: string) {
    const matchFile = this.files.find(file => file.filename === filename)!;
    matchFile.content = content;
  }
}
