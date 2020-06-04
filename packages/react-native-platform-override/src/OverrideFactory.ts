/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {
  CopyOverride,
  DerivedOverride,
  PatchOverride,
  PlatformOverride,
} from './Override';
import {OverrideFileRepository, ReactFileRepository} from './FileRepository';
import {hashContent} from './Hash';

/**
 * Allows creation of overrides, taking care of guts such as hashing, version checking, etc.
 */
export default interface OverrideFactory {
  createPlatformOverride(file: string): Promise<PlatformOverride>;

  createCopyOverride(
    file: string,
    baseFile: string,
    issue: number,
  ): Promise<CopyOverride>;

  createDerivedOverride(
    file: string,
    baseFile: string,
    issue?: number | 'LEGACY_FIXME',
  ): Promise<DerivedOverride>;

  createPatchOverride(
    file: string,
    baseFile: string,
    issue: number | 'LEGACY_FIXME',
  ): Promise<PatchOverride>;
}

/**
 * Conrete implementation of an OverrideFactory
 */
export class OverrideFactoryImpl implements OverrideFactory {
  private reactRepo: ReactFileRepository;
  private overrideRepo: OverrideFileRepository;

  constructor(
    reactRepo: ReactFileRepository,
    overrideRepo: OverrideFileRepository,
  ) {
    this.reactRepo = reactRepo;
    this.overrideRepo = overrideRepo;
  }

  async createPlatformOverride(file: string): Promise<PlatformOverride> {
    await this.checkOverrideFileExists(file);
    return new PlatformOverride({file});
  }

  async createCopyOverride(
    file: string,
    baseFile: string,
    issue: number,
  ): Promise<CopyOverride> {
    await this.checkOverrideFileExists(file);
    return new CopyOverride({
      file,
      issue,
      ...(await this.getOverrideBaseInfo(baseFile)),
    });
  }

  async createDerivedOverride(
    file: string,
    baseFile: string,
    issue?: number,
  ): Promise<DerivedOverride> {
    await this.checkOverrideFileExists(file);
    return new DerivedOverride({
      file,
      issue,
      ...(await this.getOverrideBaseInfo(baseFile)),
    });
  }

  async createPatchOverride(
    file: string,
    baseFile: string,
    issue: number | 'LEGACY_FIXME',
  ): Promise<PatchOverride> {
    await this.checkOverrideFileExists(file);
    return new PatchOverride({
      file,
      issue,
      ...(await this.getOverrideBaseInfo(baseFile)),
    });
  }

  private async checkOverrideFileExists(file: string) {
    const contents = await this.overrideRepo.getFileContents(file);
    if (contents === null) {
      throw new Error(`Could not find override '${file}'`);
    }
  }

  private async getOverrideBaseInfo(
    baseFile: string,
  ): Promise<{baseFile: string; baseVersion: string; baseHash: string}> {
    const baseContent = await this.reactRepo.getFileContents(baseFile);
    if (baseContent === null) {
      throw new Error(`Could not find base file '${baseFile}'`);
    }

    const baseVersion = this.reactRepo.getVersion();
    const baseHash = hashContent(baseContent);

    return {baseFile, baseVersion, baseHash};
  }
}
