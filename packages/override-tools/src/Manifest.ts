/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as ManifestData from './ManifestData';

import * as _ from 'lodash';
import * as crypto from 'crypto';
import * as path from 'path';

import {OverrideFileRepository, ReactFileRepository} from './FileRepository';

export interface ValidationError {
  type:
    | 'fileMissingFromManifest' // An override file is present with no manifest entry
    | 'overrideFileNotFound' // The manifest describes a file which does not exist
    | 'baseFileNotFound' // The base file for a manifest entry cannot be found
    | 'outOfDate'; // A base file has changed since the manifested version
  file: string;
}

export type OverrideType = 'platform' | 'patch' | 'derived' | 'copy';

interface OverrideBaseInfo {
  baseFile: string;
  baseVersion: string;
  baseHash: string;
}

export default class Manifest {
  private overrides: Array<ManifestData.Entry>;
  private overrideRepo: OverrideFileRepository;
  private reactRepo: ReactFileRepository;

  constructor(
    data: ManifestData.Manifest,
    overrideRepo: OverrideFileRepository,
    reactRepo: ReactFileRepository,
  ) {
    this.overrides = _.cloneDeep(data.overrides);
    this.overrideRepo = overrideRepo;
    this.reactRepo = reactRepo;

    this.overrides.forEach(override => {
      override.file = path.normalize(override.file);
      if (override.type !== 'platform') {
        override.baseFile = path.normalize(override.baseFile);
      }
    });
  }

  /**
   * Check that overrides are accurately accounted for in the manifest. I.e. we
   * should have a 1:1 mapping between files and manifest entries, and base files
   * should be present and unchanged since entry creation.
   */
  async validate(): Promise<Array<ValidationError>> {
    const errors: Array<ValidationError> = [];

    const manifestFiles = this.overrides.map(override => override.file);
    const overrideFiles = await this.overrideRepo.listFiles();

    const fileMissingFromManifest = _.difference(overrideFiles, manifestFiles);
    fileMissingFromManifest.forEach(file =>
      errors.push({type: 'fileMissingFromManifest', file: file}),
    );
    const overridesNotFound = _.difference(manifestFiles, overrideFiles);
    overridesNotFound.forEach(file =>
      errors.push({type: 'overrideFileNotFound', file: file}),
    );

    await Promise.all(
      this.overrides.map(async override => {
        if (override.type === 'platform') {
          return;
        }

        const baseFile = override.baseFile;
        const baseContent = await this.reactRepo.getFileContents(baseFile);
        if (baseContent === null) {
          errors.push({type: 'baseFileNotFound', file: override.file});
          return;
        }

        const baseHash = Manifest.hashContent(baseContent);
        if (baseHash.toLowerCase() !== override.baseHash.toLowerCase()) {
          errors.push({type: 'outOfDate', file: override.file});
          return;
        }
      }),
    );

    return errors;
  }

  /**
   * Add an override to the manifest, throwing if any part is invalid.
   */
  async addOverride(
    type: OverrideType,
    file: string,
    baseFile?: string,
    issue?: number,
  ) {
    if (['derived', 'patch', 'copy'].includes(type) && !baseFile) {
      throw new Error(`baseFile is required for ${type} overrides`);
    }

    if (['patch', 'copy'].includes(type) && !issue) {
      throw new Error(`issue is required for ${type} overrides`);
    }

    switch (type) {
      case 'platform':
        return this.addPlatformOverride(file);

      case 'derived':
        return this.addDerivedOverride(file, baseFile, issue);

      case 'patch':
        return this.addPatchOverride(file, baseFile, issue);

      case 'copy':
        return this.addCopyOverride(file, baseFile, issue);

      default:
        throw new Error(`Unknown override type '${type}'`);
    }
  }

  /**
   * Platform override specific version of {@see addOverride}
   */
  async addPlatformOverride(file: string) {
    file = await this.checkAndNormalizeOverrideFile(file);
    this.overrides.push({type: 'platform', file});
  }

  /**
   * Dervied override specific version of {@see addOverride}
   */
  async addDerivedOverride(file: string, baseFile: string, issue?: number) {
    file = await this.checkAndNormalizeOverrideFile(file);

    const overrideBaseInfo = await this.getOverrideBaseInfo(baseFile);
    this.overrides.push({type: 'derived', file, ...overrideBaseInfo, issue});
  }

  /**
   * Patch override specific version of {@see addOverride}
   */
  async addPatchOverride(file: string, baseFile: string, issue: number) {
    file = await this.checkAndNormalizeOverrideFile(file);

    const overrideBaseInfo = await this.getOverrideBaseInfo(baseFile);
    this.overrides.push({type: 'patch', file, ...overrideBaseInfo, issue});
  }

  /**
   * Copy override specific version of {@see addOverride}
   */
  async addCopyOverride(file: string, baseFile: string, issue: number) {
    file = await this.checkAndNormalizeOverrideFile(file);

    const overrideBaseInfo = await this.getOverrideBaseInfo(baseFile);
    this.overrides.push({type: 'copy', file, ...overrideBaseInfo, issue});
  }

  /**
   * Whether the manifest contains a given override
   */
  hasOverride(overridePath: string): boolean {
    return this.overrides.findIndex(ovr => ovr.file === overridePath) !== -1;
  }

  /**
   * Try to remove an override.
   * @returns false if none is found with the given name
   */
  removeOverride(overridePath: string): boolean {
    const idx = this.findOverrideIndex(overridePath);
    if (idx === -1) {
      return false;
    }

    this.overrides.splice(idx, 1);
    return true;
  }

  /**
   * Returns the entry corresponding to the given override path, or null if none
   * exists.
   */
  findOverride(overridePath: string): ManifestData.Entry | null {
    const idx = this.findOverrideIndex(overridePath);
    if (idx === -1) {
      return null;
    }

    return _.cloneDeep(this.overrides[idx]);
  }

  /**
   * Updates an override entry to mark it as up-to-date in regards to its
   * current base file.
   */
  async markUpToDate(overridePath: string) {
    const entryIdx = this.findOverrideIndex(overridePath);
    if (entryIdx === -1) {
      throw new Error(`Override at '${overridePath}' does not exist`);
    }

    const internalEntry = this.overrides[entryIdx];
    if (internalEntry.type === 'platform') {
      throw new Error('Cannot mark a platform override as up-to-date');
    }

    const newBaseInfo = await this.getOverrideBaseInfo(internalEntry.baseFile);
    Object.assign(internalEntry, internalEntry, newBaseInfo);
  }

  /**
   * Return a copy of the manifest as raw data
   */
  getAsData(): ManifestData.Manifest {
    return {overrides: _.cloneDeep(this.overrides)};
  }

  /**
   * Returns the version of React Native the manifest considers as current
   */
  currentVersion(): string {
    return this.reactRepo.getVersion();
  }

  /**
   * Hash content into the form expected in a manifest entry. Exposed for
   * testing.
   */
  static hashContent(str: string) {
    const hasher = crypto.createHash('sha1');
    hasher.update(str);
    return hasher.digest('hex');
  }

  /**
   * Find the index to a given override.
   * @returns -1 if it cannot be found
   */
  private findOverrideIndex(overridePath: string): number {
    const noramlizedPath = path.normalize(overridePath);
    return this.overrides.findIndex(ovr => ovr.file === noramlizedPath);
  }

  private async checkAndNormalizeOverrideFile(file: string): Promise<string> {
    const normalizedFile = path.normalize(file);

    if (this.hasOverride(normalizedFile)) {
      throw new Error(`Override '${file}' already exists in manifest`);
    }

    const contents = await this.overrideRepo.getFileContents(normalizedFile);
    if (contents === null) {
      throw new Error(`Could not find override '${file}'`);
    }

    return normalizedFile;
  }

  private async getOverrideBaseInfo(file: string): Promise<OverrideBaseInfo> {
    const baseFile = path.normalize(file);
    const baseContent = await this.reactRepo.getFileContents(baseFile);
    if (baseContent === null) {
      throw new Error(`Could not find base file '${file}'`);
    }

    const baseVersion = this.currentVersion();
    const baseHash = Manifest.hashContent(baseContent);

    return {baseFile, baseVersion, baseHash};
  }
}
