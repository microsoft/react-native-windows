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
import {getNpmPackage} from './PackageUtils';

const REACT_NATIVE_GITHUB_URL = 'https://github.com/facebook/react-native.git';
const DEFAULT_DIR = path.join(os.tmpdir(), getNpmPackage().name, 'git');

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
    repo.gitClient.silent(true);

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

  /**
   * Generate a Git-style patch to transform the given file into the given
   * content.
   */
  async generatePatch(
    filename: string,
    reactNativeVersion: string,
    newContent: string,
  ): Promise<string> {
    return this.actionQueue.enqueue(async () => {
      await this.checkoutVersion(reactNativeVersion);
      const filePath = path.join(this.gitDirectory, filename);

      try {
        await fs.promises.writeFile(filePath, newContent);
        const patch = await this.gitClient.diff([
          '--patch',
          '--ignore-space-at-eol',
        ]);
        return patch;
      } finally {
        await this.gitClient.reset('hard');
      }
    });
  }

  /**
   * Apply a patch to the given file, returning the merged result, which may
   * include conflict markers. The underlying file is not mutated.
   */
  async getPatchedFile(
    filename: string,
    reactNativeVersion: string,
    patchContent: string,
  ): Promise<string> {
    return this.actionQueue.enqueue(async () => {
      await this.checkoutVersion(reactNativeVersion);
      const filePath = path.join(this.gitDirectory, filename);
      const patchPath = path.join(this.gitDirectory, 'rnwgit.patch');
      try {
        await fs.promises.writeFile(patchPath, patchContent);
        try {
          await this.gitClient.raw([
            'apply',
            '--3way',
            '--whitespace=nowarn',
            patchPath,
          ]);
        } catch (ex) {
          // Hack alert: simple-git doesn't populate exception information from
          // conflicts when we're using raw commands (which we need to since it
          // doesn't support apply). Try to detect if Git gave us a bad exit code
          // because of merge conflicts, which we explicitly want to allow.
          if (!ex.message.includes('with conflicts')) {
            throw ex;
          }
        }
        const patchedFile = await fs.promises.readFile(filePath);
        return patchedFile.toString();
      } finally {
        await this.gitClient.reset('hard');
      }
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
