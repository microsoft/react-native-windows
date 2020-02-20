/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';

import {
  IOverrideFileRepository,
  IReactFileRepository,
} from 'src/FileRepositoryInterface';

export interface MockFile {
  filename: string;
  content: string;
}

export class MockReactFileRepository implements IReactFileRepository {
  private files: Array<MockFile>;

  constructor(files: Array<MockFile>) {
    this.files = files;
  }

  async getFileContents(filename: string): Promise<string | null> {
    const matches = _.filter(this.files, file => file.filename === filename);
    if (matches.length === 0) {
      return null;
    } else {
      return matches[0].content;
    }
  }
}

export class MockOverrideFileRepository implements IOverrideFileRepository {
  private files: Array<MockFile>;

  constructor(files: Array<MockFile>) {
    this.files = files;
  }

  async listFiles(): Promise<string[]> {
    return this.files.map(file => file.filename);
  }

  async getFileContents(filename: string): Promise<string | null> {
    const matches = _.filter(this.files, file => file.filename === filename);
    if (matches.length === 0) {
      return null;
    } else {
      return matches[0].content;
    }
  }
}
