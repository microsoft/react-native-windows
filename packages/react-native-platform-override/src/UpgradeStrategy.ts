/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import GitReactFileRepository from './GitReactFileRepository';
import {OverrideFileRepository} from './FileRepository';
import isutf8 from 'isutf8';

export interface UpgradeResult {
  fileWritten: boolean;
  hasConflicts: boolean;
}

/**
 * An UpgradeStrategy describes the process to upgrade an individual override
 * to a new version of the React Native source tree.
 */
export default interface UpgradeStrategy {
  upgrade(
    gitReactRepo: GitReactFileRepository,
    overrideRepo: OverrideFileRepository,
    newVersion: string,
    allowConflicts: boolean,
  ): Promise<UpgradeResult>;
}

export const UpgradeStrategies = {
  /**
   * No work needed to upgrade
   */
  assumeUpToDate: (): UpgradeStrategy => ({
    upgrade: async () => ({fileWritten: false, hasConflicts: false}),
  }),

  /**
   * Perform a three way merge of the original base file, the overriden version
   * of it, and the base file from a newwer version of React Native.
   */
  threeWayMerge: (
    override: string,
    baseFile: string,
    baseVersion: string,
  ): UpgradeStrategy => ({
    upgrade: async (gitReactRepo, overrideRepo, newVersion, allowConflicts) => {
      const ovrContent = await overrideRepo.getFileContents(override);
      if (ovrContent === null) {
        throw new Error(`Could not read ${override}`);
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
        return {fileWritten: false, hasConflicts};
      }

      const prettyPatched =
        hasConflicts && isutf8(patchedFile)
          ? Buffer.from(
              patchedFile
                .toString('utf8')
                .replace(/<<<<<<< ours/g, '<<<<<<< Upstream')
                .replace(/>>>>>>> theirs/g, '>>>>>>> Override'),
            )
          : patchedFile;

      if (!hasConflicts || allowConflicts) {
        await overrideRepo.setFileContents(override, prettyPatched);
        return {fileWritten: true, hasConflicts};
      }

      return {fileWritten: false, hasConflicts};
    },
  }),

  /**
   * Overwrite our override with base file contents
   */
  copyFile: (override: string, baseFile: string): UpgradeStrategy => ({
    upgrade: async (gitReactRepo, overrideRepo, newVersion) => {
      const newContent = await gitReactRepo.getFileContents(
        baseFile,
        newVersion,
      );
      if (newContent === null) {
        throw new Error(`Could not read ${baseFile}@${newVersion}`);
      }

      await overrideRepo.setFileContents(override, newContent);
      return {fileWritten: true, hasConflicts: false};
    },
  }),
};
