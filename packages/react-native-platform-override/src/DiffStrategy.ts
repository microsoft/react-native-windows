/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import GitReactFileRepository from './GitReactFileRepository';
import {WritableFileRepository} from './FileRepository';

/**
 * An UpgradeStrategy describes the process to diff an override to its base
 */
export default interface DiffStrategy {
  diff(
    gitReactRepo: GitReactFileRepository,
    overrideRepo: WritableFileRepository,
  ): Promise<string>;
}

export const DiffStrategies = {
  /**
   * Assume the override is the same as the original
   */
  asssumeSame: (): DiffStrategy => ({
    diff: async () => '',
  }),

  /**
   * Assume the override is the same as the original
   */
  compareBaseFile: (
    overrideFile: string,
    baseFile: string,
    baseVersion: string,
  ): DiffStrategy => ({
    diff: async (gitReactRepo, overrideRepo) => {
      const overrideContents = await overrideRepo.readFile(overrideFile);
      if (!overrideContents) {
        throw new Error(`Couldn't read override "${overrideFile}"`);
      }

      const patch = await gitReactRepo.generatePatch(
        baseFile,
        baseVersion,
        overrideContents,
      );

      return patch
        .trim()
        .split('\n')
        .slice(4) // Ignore Git gunk
        .join('\n');
    },
  }),
};
