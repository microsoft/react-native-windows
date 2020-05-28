/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import GitReactFileRepository from './GitReactFileRepository';
import {NonPlatformEntry} from './ManifestData';
import {OverrideFileRepository} from './FileRepository';

export interface UpgradeResult {
  overrideFile: string;
  hasConflicts: boolean;
  content: string;
}

export default class OverrideUpgrader {
  private reactRepo: GitReactFileRepository;
  private ovrRepo: OverrideFileRepository;

  constructor(
    reactRepo: GitReactFileRepository,
    ovrRepo: OverrideFileRepository,
  ) {
    this.reactRepo = reactRepo;
    this.ovrRepo = ovrRepo;
  }

  async getUpgraded(
    override: NonPlatformEntry,
    newVersion: string,
  ): Promise<UpgradeResult> {
    if (override.type === 'copy') {
      return {
        overrideFile: override.file,
        hasConflicts: false,
        content: await this.reactRepo.getFileContents(
          override.baseFile,
          newVersion,
        ),
      };
    }

    const ovrContent = await this.ovrRepo.getFileContents(override.file);
    const ovrAsPatch = await this.reactRepo.generatePatch(
      override.baseFile,
      override.baseVersion,
      ovrContent,
    );

    const patched = (await this.reactRepo.getPatchedFile(
      override.baseFile,
      newVersion,
      ovrAsPatch,
    ))
      .replace(/<<<<<<< ours/g, '<<<<<<< Upstream')
      .replace(/>>>>>>> theirs/g, '>>>>>>> Override');

    return {
      overrideFile: override.file,
      hasConflicts: patched.includes('<<<<<<<'),
      content: patched,
    };
  }
}
