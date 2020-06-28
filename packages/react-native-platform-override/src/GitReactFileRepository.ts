/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';
import * as semver from 'semver';
import * as simplegit from 'simple-git/promise';

import ActionQueue from './ActionQueue';
import {VersionedReactFileRepository} from './FileRepository';
import fetch from 'node-fetch';
import {getNpmPackage} from './PackageUtils';

const RN_COMMIT_ENDPOINT =
  'https://api.github.com/repos/facebook/react-native/commits';
const RN_GITHUB_URL = 'https://github.com/facebook/react-native.git';

const defaultDir = path.join(os.tmpdir(), getNpmPackage().name, 'git');

/**
 * Retrives React Native files using the React Native Github repo. Switching
 * between getting file contents of different versions may be slow.
 */
export default class GitReactFileRepository
  implements VersionedReactFileRepository {
  private gitClient: simplegit.SimpleGit;
  private gitDirectory: string;
  private checkedOutVersion?: string;

  // We have a potential race condition where one call to getFileContents
  // could checkout out a new tag while an existing call is rading a file.
  // Queue items to ensure the read operation is performed atomically
  private actionQueue: ActionQueue;

  private constructor(gitDirectory: string, gitClient: simplegit.SimpleGit) {
    this.actionQueue = new ActionQueue();
    this.gitDirectory = gitDirectory;
    this.gitClient = gitClient;
  }

  static async createAndInit(
    gitDirectory: string = defaultDir,
  ): Promise<GitReactFileRepository> {
    await fs.promises.mkdir(gitDirectory, {recursive: true});

    const gitClient = simplegit(gitDirectory);
    gitClient.silent(true);

    if (!(await gitClient.checkIsRepo())) {
      await gitClient.init();
    }

    return new GitReactFileRepository(gitDirectory, gitClient);
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
      if (!(await this.tryCheckoutLocal(reactNativeVersion))) {
        await this.fetchAndCheckout(reactNativeVersion);
      }

      this.checkedOutVersion = reactNativeVersion;
    }
  }

  private async tryCheckoutLocal(reactNativeVersion: string): Promise<boolean> {
    try {
      await this.gitClient.checkout(reactNativeVersion);
      await this.gitClient.reset('hard');
      return true;
    } catch {
      return false;
    }
  }

  private async fetchAndCheckout(reactNativeVersion: string) {
    const gitRef = await this.refFromVersion(reactNativeVersion);

    try {
      await this.gitClient.fetch([
        RN_GITHUB_URL,
        `${gitRef}:${reactNativeVersion}`,
        '--depth=1',
      ]);
      await this.gitClient.checkout(reactNativeVersion);
    } catch (ex) {
      throw new Error(`Failed to fetch '${gitRef}'. Does it exist?`);
    }
  }

  private async refFromVersion(reactNativeVersion: string): Promise<string> {
    if (!semver.valid(reactNativeVersion)) {
      throw new Error(`${reactNativeVersion} is not a valid semver version`);
    }

    // Stable releases of React Native use a tag where nightly releases embed
    // a commit hash into the prerelease tag of 0.0.0 versions
    if (semver.lt(reactNativeVersion, '0.0.0', {includePrerelease: true})) {
      // We cannot do a shallow fetch of an abbreviated commit hash
      const shortHash = semver.prerelease(reactNativeVersion)![0];
      return this.longCommitHash(shortHash);
    } else {
      return `refs/tags/v${reactNativeVersion}`;
    }
  }

  private async longCommitHash(shortHash: string): Promise<string> {
    // We cannot get long hash directly from a remote, so query Github's API
    // for it.
    const commitInfo = await fetch(`${RN_COMMIT_ENDPOINT}/${shortHash}`);
    if (!commitInfo.ok) {
      throw new Error(`Unable to query Github for commit '${shortHash}`);
    }

    return (await commitInfo.json()).sha;
  }
}
