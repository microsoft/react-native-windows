/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as path from 'path';
import GitReactFileRepository from './GitReactFileRepository';
import {WritableFileRepository} from './FileRepository';
import isutf8 from 'isutf8';

export interface UpgradeResult {
  overrideName: string;
  filesWritten: boolean;
  hasConflicts: boolean;
}

/**
 * An UpgradeStrategy describes the process to upgrade an individual override
 * to a new version of the React Native source tree.
 */
export default interface UpgradeStrategy {
  upgrade(
    gitReactRepo: GitReactFileRepository,
    overrideRepo: WritableFileRepository,
    newVersion: string,
    allowConflicts: boolean,
  ): Promise<UpgradeResult>;
}

export const UpgradeStrategies = {
  /**
   * No work needed to upgrade
   */
  assumeUpToDate: (overrideName: string): UpgradeStrategy => ({
    upgrade: async () => ({
      overrideName,
      filesWritten: false,
      hasConflicts: false,
    }),
  }),

  /**
   * Perform a three way merge of the original base file, the overriden version
   * of it, and the base file from a newwer version of React Native.
   */
  threeWayMerge: (
    overrideName: string,
    baseFile: string,
    baseVersion: string,
  ): UpgradeStrategy => ({
    upgrade: async (gitReactRepo, overrideRepo, newVersion, allowConflicts) => {
      const ovrContent = await overrideRepo.readFile(overrideName);
      if (ovrContent === null) {
        throw new Error(`Could not read ${overrideName}`);
      }

      const ovrAsPatch = await gitReactRepo.generatePatch(
        baseFile,
        baseVersion,
        ovrContent,
      );

      const {patchedFile, hasConflicts} = await gitReactRepo.getPatchedFile(
        baseFile,
        newVersion,
        ovrAsPatch,
      );

      if (!patchedFile) {
        return {overrideName, filesWritten: false, hasConflicts};
      }

      const prettyPatched =
        hasConflicts && isutf8(patchedFile)
          ? patchedFile
              .toString('utf8')
              .replace(/<<<<<<< ours/g, '<<<<<<< Upstream')
              .replace(/>>>>>>> theirs/g, '>>>>>>> Override')
          : patchedFile;

      if (!hasConflicts || allowConflicts) {
        await overrideRepo.writeFile(overrideName, prettyPatched);
        return {overrideName, filesWritten: true, hasConflicts};
      }

      return {overrideName, filesWritten: false, hasConflicts};
    },
  }),

  /**
   * Overwrite our override with base file contents
   */
  copyFile: (overrideName: string, baseFile: string): UpgradeStrategy => ({
    upgrade: async (gitReactRepo, overrideRepo, newVersion) => {
      const newContent = await gitReactRepo.readFile(baseFile, newVersion);
      if (newContent === null) {
        throw new Error(`Could not read ${baseFile}@${newVersion}`);
      }

      await overrideRepo.writeFile(overrideName, newContent);
      return {overrideName, filesWritten: true, hasConflicts: false};
    },
  }),

  /**
   * Overwrite our override with base file contents
   */
  copyDirectory: (
    overrideDirectory: string,
    baseDirectory: string,
  ): UpgradeStrategy => ({
    upgrade: async (gitReactRepo, overrideRepo, newVersion, allowConflicts) => {
      const baseFiles = await gitReactRepo.listFiles(
        [`${baseDirectory}/**`],
        newVersion,
      );

      for (const baseFile of baseFiles) {
        const relative = path.relative(baseDirectory, baseFile);
        await UpgradeStrategies.copyFile(
          path.join(overrideDirectory, relative),
          baseFile,
        ).upgrade(gitReactRepo, overrideRepo, newVersion, allowConflicts);
      }

      return {
        overrideName: overrideDirectory,
        filesWritten: true,
        hasConflicts: false,
      };
    },
  }),
};
