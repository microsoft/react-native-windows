/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';
import * as simplegit from 'simple-git/promise';

import ActionQueue from './ActionQueue';
import {VersionedReactFileRepository} from './FileRepository';

const REACT_NATIVE_GITHUB_URL = 'https://github.com/facebook/react-native.git';
const DEFAULT_DIR = path.join(os.tmpdir(), 'react-native-windows-override-git');

/**
 * Retrives React Native files using the React Native Github repo. Switching
 * between getting file contents of different versions may be slow.
 */
export default class GitReactFileRepository
  implements VersionedReactFileRepository {
  private gitClient: simplegit.SimpleGit;
  private gitDirectory: string;
  private checkedOutVersion: string;

  // We have a potential race condition where one call to getFileContents
  // could checkout out a new tag while an existing call is rading a file.
  // Queue items to ensure the read operation is performed atomically
  private actionQueue: ActionQueue;

  private constructor() {}

  static async createAndInit(
    gitDirectory?: string,
  ): Promise<GitReactFileRepository> {
    let repo = new GitReactFileRepository();
    repo.actionQueue = new ActionQueue();

    repo.gitDirectory = gitDirectory || DEFAULT_DIR;
    await fs.promises.mkdir(repo.gitDirectory, {recursive: true});

    repo.gitClient = simplegit(repo.gitDirectory);
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
    return this.actionQueue.enqueue(async () => {
      await this.checkoutVersion(reactNativeVersion);
      const filePath = path.join(this.gitDirectory, filename);

      try {
        return (await fs.promises.readFile(filePath)).toString();
      } catch {
        return null;
      }
    });
  }

  async generatePatch(
    filename: string,
    reactNativeVersion: string,
    newContent: string,
  ): Promise<string> {
    return this.actionQueue.enqueue(async () => {
      await this.checkoutVersion(reactNativeVersion);
      const filePath = path.join(this.gitDirectory, filename);

      await fs.promises.writeFile(filePath, newContent);
      const patch = await this.gitClient.diff([
        '--patch',
        '--ignore-space-at-eol',
      ]);

      await this.gitClient.reset('hard');
      return patch;
    });
  }

  async applyPatchToFile(
    filename: string,
    reactNativeVersion: string,
    patchContent: string,
  ): Promise<string> {
    return this.actionQueue.enqueue(async () => {
      await this.checkoutVersion(reactNativeVersion);
      const filePath = path.join(this.gitDirectory, filename);

      const patchPath = path.join(this.gitDirectory, 'rnwgit.patch');
      await fs.promises.writeFile(patchPath, patchContent);
      await this.gitClient.raw(['apply', '--3way', patchPath]);
      const patchedFile = await fs.promises.readFile(filePath);

      await this.gitClient.reset('hard');
      return patchedFile.toString();
    });
  }

  private async checkoutVersion(reactNativeVersion: string) {
    if (reactNativeVersion !== this.checkedOutVersion) {
      try {
        await this.gitClient.fetch(
          'origin',
          `refs/tags/v${reactNativeVersion}`,
        );
        await this.gitClient.checkout('FETCH_HEAD');
        await this.gitClient.reset('hard');
        this.checkedOutVersion = reactNativeVersion;
      } catch (ex) {
        throw new Error(
          `Failed to checkout tag 'v${reactNativeVersion}'. Does it exist?`,
        );
      }
    }
  }
}
