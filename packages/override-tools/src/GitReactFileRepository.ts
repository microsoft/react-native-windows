/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';
import * as simplegit from 'simple-git/promise';

import ActionQueue from './ActionQueue';
import {VersionedReactFileRepository} from './FileRepository';

const REACT_NATIVE_GITHUB_URL = 'https://github.com/facebook/react-native.git';

/**
 * Retrives React Native files using the React Native Github repo. Switching
 * between getting file contents of different versions may be slow.
 */
export default class GitReactFileRepository
  implements VersionedReactFileRepository {
  private gitClient: simplegit.SimpleGit;
  private gitDirectory: string;
  private checkedOutVersion: string;
  private actionQueue: ActionQueue<string | null>;

  private constructor() {}

  static async createAndInit(
    gitDirectory: string,
  ): Promise<GitReactFileRepository> {
    let repo = new GitReactFileRepository();
    repo.gitDirectory = gitDirectory;
    repo.actionQueue = new ActionQueue();

    await fs.promises.mkdir(gitDirectory, {recursive: true});

    repo.gitClient = simplegit(gitDirectory);
    if (!(await repo.gitClient.checkIsRepo())) {
      await repo.gitClient.init();
      await repo.gitClient.addRemote('origin', REACT_NATIVE_GITHUB_URL);
    }

    return repo;
  }

  async getFileContents(
    filename: string,
    reactNativeVersion: string,
  ): Promise<string | null> {
    // We have a potential race condition where one call to getFileContents
    // could checkout out a new tag while an existing call is rading a file.
    // Queue items to ensure the read operation is performed atomically
    return this.actionQueue.enqueue(() => {
      return this.getFileContentsNonAtomic(filename, reactNativeVersion);
    });
  }

  private async getFileContentsNonAtomic(
    filename: string,
    reactNativeVersion: string,
  ): Promise<string | null> {
    await this.checkoutVersion(reactNativeVersion);
    const filePath = path.join(this.gitDirectory, filename);

    try {
      return (await fs.promises.readFile(filePath)).toString();
    } catch {
      return null;
    }
  }

  private async checkoutVersion(reactNativeVersion: string) {
    if (reactNativeVersion !== this.checkedOutVersion) {
      try {
        await this.gitClient.fetch(
          'origin',
          `refs/tags/v${reactNativeVersion}`,
        );
        await this.gitClient.checkout('FETCH_HEAD');
        this.checkedOutVersion = reactNativeVersion;
      } catch (ex) {
        throw new Error(
          `Failed to checkout tag 'v${reactNativeVersion}'. Does it exist?`,
        );
      }
    }
  }
}
