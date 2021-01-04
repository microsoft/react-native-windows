/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as path from 'path';

import {
  CopyOverride,
  DerivedOverride,
  DirectoryCopyOverride,
  PatchOverride,
  PlatformOverride,
} from './Override';
import FileRepository, {ReactFileRepository} from './FileRepository';
import {hashFileOrDirectory} from './Hash';

/**
 * Allows creation of overrides, taking care of guts such as hashing, version checking, etc.
 */
export default interface OverrideFactory {
  createPlatformOverride(file: string): Promise<PlatformOverride>;

  createCopyOverride(
    file: string,
    baseFile: string,
    issue?: number,
  ): Promise<CopyOverride>;

  createDerivedOverride(
    file: string,
    baseFile: string,
    issue?: number,
  ): Promise<DerivedOverride>;

  createPatchOverride(
    file: string,
    baseFile: string,
    issue?: number,
  ): Promise<PatchOverride>;

  createDirectoryCopyOverride(
    directory: string,
    baseDirectory: string,
    issue?: number,
  ): Promise<DirectoryCopyOverride>;
}

/**
 * Conrete implementation of an OverrideFactory
 */
export class OverrideFactoryImpl implements OverrideFactory {
  private readonly reactRepo: ReactFileRepository;
  private readonly overrideRepo: FileRepository;

  constructor(reactRepo: ReactFileRepository, overrideRepo: FileRepository) {
    this.reactRepo = reactRepo;
    this.overrideRepo = overrideRepo;
  }

  async createPlatformOverride(file: string): Promise<PlatformOverride> {
    await this.checkOverrideExists(file, 'file');
    return new PlatformOverride({file});
  }

  async createCopyOverride(
    file: string,
    baseFile: string,
    issue?: number,
  ): Promise<CopyOverride> {
    await this.checkOverrideExists(file, 'file');
    return new CopyOverride({
      file,
      baseFile,
      issue,
      ...(await this.getBaseInfo(baseFile)),
    });
  }

  async createDerivedOverride(
    file: string,
    baseFile: string,
    issue?: number,
  ): Promise<DerivedOverride> {
    await this.checkOverrideExists(file, 'file');
    return new DerivedOverride({
      file,
      baseFile,
      issue,
      ...(await this.getBaseInfo(baseFile)),
    });
  }

  async createPatchOverride(
    file: string,
    baseFile: string,
    issue?: number,
  ): Promise<PatchOverride> {
    await this.checkOverrideExists(file, 'file');
    return new PatchOverride({
      file,
      baseFile,
      issue,
      ...(await this.getBaseInfo(baseFile)),
    });
  }

  async createDirectoryCopyOverride(
    directory: string,
    baseDirectory: string,
    issue?: number,
  ): Promise<DirectoryCopyOverride> {
    await this.checkOverrideExists(directory, 'directory');
    return new DirectoryCopyOverride({
      directory,
      baseDirectory,
      issue,
      ...(await this.getBaseInfo(baseDirectory)),
    });
  }

  private async checkOverrideExists(
    overridePath: string,
    type: 'file' | 'directory',
  ) {
    if (path.isAbsolute(overridePath)) {
      throw new Error(
        `Expected override path to be repo relative. Got '${overridePath}'`,
      );
    }

    if ((await this.overrideRepo.stat(overridePath)) !== type) {
      throw new Error(
        `Could not find ${type} at repo relative path '${overridePath}'`,
      );
    }
  }

  private async getBaseInfo(
    basePath: string,
  ): Promise<{baseVersion: string; baseHash: string}> {
    if (path.isAbsolute(basePath)) {
      throw new Error(
        `Expected base path to be repo relative. Got '${basePath}'`,
      );
    }

    const baseType = await this.reactRepo.stat(basePath);
    if (baseType === 'none') {
      throw new Error(`Could not find base file/directory '${basePath}'`);
    }

    const baseVersion = this.reactRepo.getVersion();
    const baseHash = (await hashFileOrDirectory(basePath, this.reactRepo))!;

    return {baseVersion, baseHash};
  }
}
