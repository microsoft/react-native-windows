/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from 'fs';
import os from 'os';
import path from 'path';
import simplegit from 'simple-git/promise';

import BatchingQueue from './BatchingQueue';
import FileSystemRepository from './FileSystemRepository';
import {VersionedReactFileRepository} from './FileRepository';
import {getNpmPackage} from './PackageUtils';
import {fetchFullRef} from './refFromVersion';

const RN_GITHUB_URL = 'https://github.com/facebook/react-native.git';

/**
 * Retrives React Native files using the React Native Github repo. Switching
 * between getting file contents of different versions may be slow.
 */
export default class GitReactFileRepository
  implements VersionedReactFileRepository {
  private readonly fileRepo: FileSystemRepository;
  private readonly gitClient: simplegit.SimpleGit;
  private checkedOutVersion?: string;
  private static githubToken?: string;

  // We need to ensure it is impossible to check out a new React Native
  // version while an operation hasn't yet finished. We queue each operation to
  // ensure they are performed atomically.
  private readonly batchingQueue: BatchingQueue<string>;

  private constructor(gitDirectory: string, gitClient: simplegit.SimpleGit) {
    this.batchingQueue = new BatchingQueue();
    this.fileRepo = new FileSystemRepository(gitDirectory);
    this.gitClient = gitClient;
  }

  /**
   * Asynchronusly initialize the scratch repository, creating a new Git repo is needed
   * @param gitDirectory optional repo directory
   */
  static async createAndInit(
    gitDirectory?: string,
  ): Promise<GitReactFileRepository> {
    const dir = gitDirectory || (await this.defaultGitDirectory());
    await fs.promises.mkdir(dir, {recursive: true});

    const gitClient = simplegit(dir);
    gitClient.silent(true);

    if (!(await gitClient.checkIsRepo())) {
      await gitClient.init();
      await gitClient.addConfig('core.autocrlf', 'input');
      await gitClient.addConfig('core.filemode', 'false');
      await gitClient.addConfig('core.ignorecase', 'true');
    }

    return new GitReactFileRepository(dir, gitClient);
  }

  /**
   * Set a GitHub API token for all instances of GitReactFileRepository to use
   * when making requests.
   * @param token a GitHub PAT
   */
  static setGithubToken(token: string) {
    GitReactFileRepository.githubToken = token;
  }

  async listFiles(
    globs: string[] | undefined,
    reactNativeVersion: string,
  ): Promise<string[]> {
    return this.usingVersion(reactNativeVersion, () =>
      this.fileRepo.listFiles(globs),
    );
  }

  async readFile(
    filename: string,
    reactNativeVersion: string,
  ): Promise<Buffer | null> {
    return this.usingVersion(reactNativeVersion, () =>
      this.fileRepo.readFile(filename),
    );
  }

  async stat(
    filename: string,
    reactNativeVersion: string,
  ): Promise<'file' | 'directory' | 'none'> {
    return this.usingVersion(reactNativeVersion, () =>
      this.fileRepo.stat(filename),
    );
  }

  /**
   * Generate a Git-style patch to transform the given file into the given
   * content.
   */
  async generatePatch(
    filename: string,
    reactNativeVersion: string,
    newContent: Buffer,
  ): Promise<string> {
    return this.usingVersion(reactNativeVersion, async () => {
      await this.ensureFile(filename);

      try {
        await this.fileRepo.writeFile(filename, newContent);
        const patch = await this.gitClient.diff([
          '--patch',
          '--ignore-space-at-eol',
          '--binary',
          '--',
          filename,
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
   *
   * Git is unable to generate a representation with conflict markers in the
   * event of binary merge conflicts. In this case a null Buffer is returned.
   */
  async getPatchedFile(
    filename: string,
    reactNativeVersion: string,
    patchContent: string,
  ): Promise<{patchedFile: Buffer | null; hasConflicts: boolean}> {
    return this.usingVersion(reactNativeVersion, async () => {
      await this.ensureFile(filename);

      try {
        await this.fileRepo.writeFile('rnwgit.patch', patchContent);

        let hasConflicts = false;
        let binaryConflicts = false;

        try {
          await this.gitClient.raw([
            'apply',
            '--3way',
            '--whitespace=nowarn',
            'rnwgit.patch',
          ]);
        } catch (ex) {
          // Hack alert: simple-git doesn't populate exception information from
          // conflicts when we're using raw commands (which we need to since it
          // doesn't support apply). Try to detect if Git gave us a bad exit code
          // because of merge conflicts, which we explicitly want to allow.
          if (!(ex as Error).message.includes('with conflicts')) {
            throw ex;
          }

          hasConflicts = true;
          binaryConflicts = (ex as Error).message.includes(
            'Cannot merge binary files',
          );
        }

        const patchedFile = binaryConflicts
          ? null
          : await this.fileRepo.readFile(filename);

        return {patchedFile, hasConflicts};
      } finally {
        await this.gitClient.reset('hard');
      }
    });
  }

  private async usingVersion<T>(
    reactNativeVersion: string,
    fn: () => Promise<T>,
  ): Promise<T> {
    return await this.batchingQueue.enqueue(reactNativeVersion, async () => {
      await this.checkoutVersion(reactNativeVersion);
      return await fn();
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
      await this.gitClient.checkout([reactNativeVersion, '--force']);
      return true;
    } catch {
      return false;
    }
  }

  private async fetchAndCheckout(reactNativeVersion: string) {
    const githubToken =
      GitReactFileRepository.githubToken ||
      process.env.PLATFORM_OVERRIDE_GITHUB_TOKEN;
    const gitRef = await fetchFullRef(reactNativeVersion, {githubToken});

    try {
      await this.gitClient.fetch([
        RN_GITHUB_URL,
        `${gitRef}:${reactNativeVersion}`,
        '--depth=1',
      ]);
    } catch (ex) {
      throw new Error(
        `Failed to fetch '${gitRef}'. Does it exist? (${
          (ex as Error).message
        })`,
      );
    }

    await this.gitClient.checkout([reactNativeVersion, '--force']);
  }

  private static async defaultGitDirectory(): Promise<string> {
    return path.join(os.tmpdir(), (await getNpmPackage()).name, 'git');
  }

  private async ensureFile(filename: string): Promise<void> {
    const stat = await this.fileRepo.stat(filename);
    if (stat === 'none') {
      throw new Error(
        `Cannot find file "${filename}" in react-native@${this.checkedOutVersion}`,
      );
    } else if (stat === 'directory') {
      throw new Error(`"${filename}" refers to a directory`);
    }
  }
}
