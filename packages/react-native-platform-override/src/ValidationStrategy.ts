/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {HashOpts, hashFileOrDirectory} from './Hash';
import {ReactFileRepository, WritableFileRepository} from './FileRepository';

export interface ValidationError {
  /**
   * Why did validation fail?
   */
  type:
    | 'missingFromManifest' // An override was found that isn't listed in the manifest
    | 'overrideNotFound' // The manifest describes a file which does not exist
    | 'baseNotFound' // The base file for a manifest entry cannot be found
    | 'expectedFile' // Expected the override to be a file but found a directory
    | 'expectedDirectory' // Expected the override to be a directory but found a file
    | 'outOfDate' // A base file has changed since the manifested version
    | 'overrideDifferentFromBase' // An override file is not an exact copy of the base file
    | 'overrideSameAsBase'; // An override file is an exact copy of the base file

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
    overrideRepo: WritableFileRepository,
    reactRepo: ReactFileRepository,
  ): Promise<ValidationError[]>;
}

export const ValidationStrategies = {
  /**
   * Validate that an override file exists
   */
  overrideFileExists: (overrideName: string): ValidationStrategy => ({
    validate: async overrideRepo => {
      switch (await overrideRepo.stat(overrideName)) {
        case 'file':
          return [];
        case 'directory':
          return [{type: 'expectedFile', overrideName}];
        case 'none':
          return [{type: 'overrideNotFound', overrideName}];
      }
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
      switch (await reactRepo.stat(baseFile)) {
        case 'file':
          return [];
        case 'directory':
          return [{type: 'expectedFile', overrideName}];
        case 'none':
          return [{type: 'baseNotFound', overrideName}];
      }
    },
  }),

  /**
   * Validate that an override directory exists
   */
  overrideDirectoryExists: (overrideName: string): ValidationStrategy => ({
    validate: async overrideRepo => {
      switch (await overrideRepo.stat(overrideName)) {
        case 'file':
          return [{type: 'expectedDirectory', overrideName}];
        case 'directory':
          return [];
        case 'none':
          return [{type: 'overrideNotFound', overrideName}];
      }
    },
  }),

  /**
   * Validate that a base directory exists
   */
  baseDirectoryExists: (
    overrideName: string,
    baseDirectory: string,
  ): ValidationStrategy => ({
    validate: async (_, reactRepo) => {
      switch (await reactRepo.stat(baseDirectory)) {
        case 'file':
          return [{type: 'expectedDirectory', overrideName}];
        case 'directory':
          return [];
        case 'none':
          return [{type: 'baseNotFound', overrideName}];
      }
    },
  }),

  /**
   * Validate that a base file/folder matches an expected hash if it exists
   */
  baseUpToDate: (
    overrideName: string,
    base: string,
    expectedBaseHash: string,
  ): ValidationStrategy => ({
    validate: async (_, reactRepo) => {
      const hash = await hashFileOrDirectory(base, reactRepo);
      if (!hash) {
        return [];
      }

      return hash === expectedBaseHash
        ? []
        : [{type: 'outOfDate', overrideName}];
    },
  }),

  /**
   * Validate that an override meant to be a copy of a base file/folder has not
   * been tampered with
   */
  overrideCopyOfBase: (
    overrideName: string,
    base: string,
  ): ValidationStrategy => ({
    validate: async (overrideRepo, reactRepo) => {
      if (
        (await overrideRepo.stat(overrideName)) === 'none' ||
        (await reactRepo.stat(base)) === 'none'
      ) {
        return [];
      }

      const overrideHash = await hashFileOrDirectory(
        overrideName,
        overrideRepo,
      );
      const baseHash = await hashFileOrDirectory(base, reactRepo);
      return overrideHash === baseHash
        ? []
        : [{type: 'overrideDifferentFromBase', overrideName}];
    },
  }),

  /**
   * Validate that an override assumed to be different from its base is not
   * identical.
   */
  overrideDifferentFromBase: (
    overrideName: string,
    base: string,
  ): ValidationStrategy => ({
    validate: async (overrideRepo, reactRepo) => {
      if (
        (await overrideRepo.stat(overrideName)) === 'none' ||
        (await reactRepo.stat(base)) === 'none'
      ) {
        return [];
      }

      // There can be whitespace differences from merges that lead to
      // semantically indentical files. Do a whitespace insensitive compare to
      // determine if there is a difference.
      const hashOpts: HashOpts = {insensitivity: 'whitespace'};

      const overrideHash = await hashFileOrDirectory(
        overrideName,
        overrideRepo,
        hashOpts,
      );
      const baseHash = await hashFileOrDirectory(base, reactRepo, hashOpts);
      return overrideHash === baseHash
        ? [{type: 'overrideSameAsBase', overrideName}]
        : [];
    },
  }),
};
