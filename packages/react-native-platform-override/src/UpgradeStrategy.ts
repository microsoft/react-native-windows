/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import GitReactFileRepository from './GitReactFileRepository';
import {OverrideFileRepository} from './FileRepository';

export interface UpgradeResult {
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
    upgrade: async () => ({hasConflicts: false}),
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

      const patched = (await gitReactRepo.getPatchedFile(
        baseFile,
        newVersion,
        ovrAsPatch,
      ))
        .replace(/<<<<<<< ours/g, '<<<<<<< Upstream')
        .replace(/>>>>>>> theirs/g, '>>>>>>> Override');

      const hasConflicts = patched.includes('<<<<<<<');
      if (!hasConflicts || allowConflicts) {
        await overrideRepo.setFileContents(override, patched);
      }

      return {hasConflicts};
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
      return {hasConflicts: false};
    },
  }),
};
