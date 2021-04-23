/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import * as path from 'path';
import {WritableFileRepository, bindVersion} from './FileRepository';
import GitReactFileRepository from './GitReactFileRepository';
import {hashFileOrDirectory} from './Hash';
import isUtf8 from 'isutf8';

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

      if (ovrAsPatch.length === 0) {
        throw new Error(`Generated patch for ${overrideName} was empty`);
      }

      const {patchedFile, hasConflicts} = await gitReactRepo.getPatchedFile(
        baseFile,
        newVersion,
        ovrAsPatch,
      );

      if (!patchedFile) {
        return {overrideName, filesWritten: false, hasConflicts};
      }

      const prettyPatched =
        hasConflicts && isUtf8(patchedFile)
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
    upgrade: async (gitReactRepo, overrideRepo, newVersion) => {
      const baseFiles = (
        await gitReactRepo.listFiles([`${baseDirectory}/**`], newVersion)
      ).map(f => path.relative(baseDirectory, f));

      const overrideFiles = (
        await overrideRepo.listFiles([`${overrideDirectory}/**`])
      ).map(f => path.relative(overrideDirectory, f));

      // Note that this logic can lead emopty directories. This shouldn't
      // matter in practice as Git won't track them.
      const deleteTasks = _.difference(overrideFiles, baseFiles).map(f =>
        overrideRepo.deleteFile(path.join(overrideDirectory, f)),
      );

      const baseRepo = bindVersion(gitReactRepo, newVersion);
      const copyTasks = baseFiles.map(async f => {
        const basePath = path.join(baseDirectory, f);
        const overridePath = path.join(overrideDirectory, f);

        // Con't replace files of the same content but different line endings
        if (
          (await hashFileOrDirectory(basePath, baseRepo)) !==
          (await hashFileOrDirectory(overridePath, overrideRepo))
        ) {
          const content = await baseRepo.readFile(basePath);
          await overrideRepo.writeFile(overridePath, content!);
        }
      });

      await Promise.all([...deleteTasks, ...copyTasks]);

      return {
        overrideName: overrideDirectory,
        filesWritten: true,
        hasConflicts: false,
      };
    },
  }),
};
