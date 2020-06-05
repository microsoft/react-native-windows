/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {OverrideFileRepository, ReactFileRepository} from './FileRepository';
import {hashContent} from './Hash';

export interface ValidationError {
  /**
   * Why did validation fail?
   */
  type:
    | 'missingFromManifest' // An override was found that isn't listed in the manifest
    | 'overrideNotFound' // The manifest describes a file which does not exist
    | 'baseNotFound' // The base file for a manifest entry cannot be found
    | 'outOfDate' // A base file has changed since the manifested version
    | 'overrideDifferentFromBase'; // An override file is not an expact copy of the base file

  /**
   * What override failed validation
   */
  overrideName: string;
}

/**
 * A ValidationStrategy allows describing a process to ensure a specific
 * override meets constraints such as ensuring file existence, preventing
 * content modification, or being up to date.
 */
export default interface ValidationStrategy {
  validate(
    overrideRepo: OverrideFileRepository,
    reactRepo: ReactFileRepository,
  ): Promise<ValidationError[]>;
}

export const ValidationStrategies = {
  /**
   * Validate that an override file exists
   */
  overrideFileExists: (overrideName: string): ValidationStrategy => ({
    validate: async overrideRepo => {
      return (await overrideRepo.getFileContents(overrideName))
        ? []
        : [{type: 'overrideNotFound', overrideName}];
    },
  }),

  /**
   * Validate that a base file exists
   */
  baseFileExists: (
    overrideName: string,
    baseFile: string,
  ): ValidationStrategy => ({
    validate: async (_, reactRepo) => {
      return (await reactRepo.getFileContents(baseFile))
        ? []
        : [{type: 'baseNotFound', overrideName}];
    },
  }),

  /**
   * Validate that a base file matches an expected hash if it exists
   */
  baseFileUpToDate: (
    overrideName: string,
    baseFile: string,
    expectedBaseHash: string,
  ): ValidationStrategy => ({
    validate: async (_, reactRepo) => {
      const contents = await reactRepo.getFileContents(baseFile);
      if (!contents) {
        return [];
      }

      return hashContent(contents) === expectedBaseHash
        ? []
        : [{type: 'outOfDate', overrideName}];
    },
  }),

  /**
   * Validate that an override meant to be a copy of a base file has not been
   * tampered with
   */
  overrideCopyOfBaseFile: (
    overrideName: string,
    baseFile: string,
  ): ValidationStrategy => ({
    validate: async (overrideRepo, reactRepo) => {
      const overrideContent = await overrideRepo.getFileContents(overrideName);
      const baseContent = await reactRepo.getFileContents(baseFile);
      if (overrideContent === null || baseContent === null) {
        return [];
      }

      // Run through hash to account for line-endings, etc
      return hashContent(overrideContent) === hashContent(baseContent)
        ? []
        : [{type: 'overrideDifferentFromBase', overrideName}];
    },
  }),
};
