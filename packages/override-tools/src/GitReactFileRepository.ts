/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from './fs-promise';
import * as path from 'path';
import * as simplegit from 'simple-git/promise';

import {IVersionedReactFileRepository} from './FileRepositoryInterface';

const REACT_NATIVE_GITHUB_URL = 'https://github.com/facebook/react-native.git';

/**
 * Retrives React Native files using the React Native Github repo. Switching
 * between getting file contents of different versions may be slow.
 */
export class GitReactFileRepository implements IVersionedReactFileRepository {
  private gitClient: simplegit.SimpleGit;
  private gitDirectory: string;

  private constructor() {}

  static async createAndInit(
    gitDirectory: string,
  ): Promise<GitReactFileRepository> {
    let reactFileRepo = new GitReactFileRepository();
    reactFileRepo.gitDirectory = gitDirectory;

    await fs.mkdir(gitDirectory, {recursive: true});

    const gitClient = (reactFileRepo.gitClient = simplegit(gitDirectory));
    if (await gitClient.checkIsRepo()) {
      await gitClient.fetch();
    } else {
      await gitClient.clone(REACT_NATIVE_GITHUB_URL, gitDirectory);
    }

    return reactFileRepo;
  }

  async getFileContents(
    filename: string,
    reactNativeVersion: string,
  ): Promise<string | null> {
    await this.gitClient.checkout(`v${reactNativeVersion}`);

    const filePath = path.join(this.gitDirectory, filename);
    if (await fs.exists(filePath)) {
      return (await fs.readFile(filePath)).toString();
    } else {
      return null;
    }
  }
}
