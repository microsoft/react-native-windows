/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Serialized from './Serialized';
import * as _ from 'lodash';
import * as path from 'path';

import Override, {deserializeOverride} from './Override';
import {ReactFileRepository, WritableFileRepository} from './FileRepository';
import OverrideFactory from './OverrideFactory';
import {ValidationError} from './ValidationStrategy';
import {eachLimit} from 'async';
import {normalizePath} from './PathUtils';

/**
 * Represents a collection of overrides listed in an on-disk manifest. Allows
 * performing aggregate operations on the overrides.
 */
export default class Manifest {
  private readonly includePatterns?: string[];
  private readonly excludePatterns?: string[];
  private baseVersion?: string;
  private readonly overrides: Override[];

  /**
   * Construct the manifest
   *
   * @param overrides List of overrides to evaluate
   * @param opts Allows specifying globs to include or exclude paths to enforce
   * exist in the manifest
   */
  constructor(
    overrides: Override[],
    opts: {
      includePatterns?: string[];
      excludePatterns?: string[];
      baseVersion?: string;
    } = {},
  ) {
    const uniquelyNamed = _.uniqBy(overrides, ovr => ovr.name());
    if (uniquelyNamed.length !== overrides.length) {
      throw new Error('Cannot construct a manifest with duplicate overrides');
    }

    this.includePatterns = opts.includePatterns;
    this.excludePatterns = opts.excludePatterns;
    this.baseVersion = opts.baseVersion;
    this.overrides = _.clone(overrides);
  }

  static fromSerialized(man: Serialized.Manifest): Manifest {
    const overrides = man.overrides.map(ovr =>
      deserializeOverride(ovr, {defaultBaseVersion: man.baseVersion}),
    );
    return new Manifest(overrides, {
      includePatterns: man.includePatterns,
      excludePatterns: man.excludePatterns,
      baseVersion: man.baseVersion,
    });
  }

  /**
   * Check that overrides are accurately accounted for in the manifest. E.g.
   * all files should be accounted for, and base files should be up to date
   * with upstream.
   */
  async validate(
    overrideRepo: WritableFileRepository,
    reactRepo: ReactFileRepository,
  ): Promise<ValidationError[]> {
    const errors: ValidationError[] = [];

    const globs = [
      ...(this.includePatterns || ['**']),
      ...(this.excludePatterns || []).map(p => '!' + p),
    ];

    const overrideFiles = await overrideRepo.listFiles(globs);
    const missingFromManifest = overrideFiles.filter(
      file =>
        file !== 'overrides.json' &&
        path.relative('node_modules', file).startsWith('..') &&
        !this.overrides.some(override => override.includesFile(file)),
    );
    for (const missingFile of missingFromManifest) {
      errors.push({type: 'missingFromManifest', overrideName: missingFile});
    }

    const validationTasks = _.flatMap(this.overrides, ovr =>
      ovr.validationStrategies(),
    );

    // eslint-disable-next-line @typescript-eslint/no-misused-promises
    await eachLimit(validationTasks, 30, async task => {
      errors.push(...(await task.validate(overrideRepo, reactRepo)));
    });

    return errors.sort((a, b) =>
      a.overrideName.localeCompare(b.overrideName, 'en'),
    );
  }

  /**
   * Add an override to the manifest
   */
  addOverride(override: Override) {
    if (this.hasOverride(override.name())) {
      throw new Error(`Trying to add duplicate override '${override.name()}'`);
    }

    this.overrides.push(override);
  }

  /**
   * Whether the manifest contains a given override
   */
  hasOverride(overrideName: string): boolean {
    return this.overrides.some(
      ovr => ovr.name() === normalizePath(overrideName),
    );
  }

  /**
   * Try to remove an override.
   * @returns false if none is found with the given name
   */
  removeOverride(overrideName: string): boolean {
    const idx = this.findOverrideIndex(overrideName);
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
  findOverride(overrideName: string): Override | null {
    const idx = this.findOverrideIndex(overrideName);
    if (idx === -1) {
      return null;
    }

    return this.overrides[idx];
  }

  /**
   * Updates an override entry to mark it as up-to-date in regards to its
   * current base file.
   */
  async markUpToDate(overrideName: string, overrideFactory: OverrideFactory) {
    const override = this.findOverride(overrideName);
    if (override === null) {
      throw new Error(`Override '${overrideName}' does not exist`);
    }

    // Mutate the object instead of replacing by index because the index may no
    // longer be the same after awaiting.
    const upToDateOverride = await override.createUpdated(overrideFactory);
    Object.assign(override, upToDateOverride);
  }

  /**
   * Return a serialized representation of the manifest
   */
  serialize(): Serialized.Manifest {
    return {
      includePatterns: this.includePatterns,
      excludePatterns: this.excludePatterns,
      baseVersion: this.baseVersion,
      overrides: this.overrides
        .sort((a, b) => a.name().localeCompare(b.name(), 'en'))
        .map(override =>
          override.serialize({defaultBaseVersion: this.baseVersion}),
        ),
    };
  }

  /**
   * Returns the overrides in the manfest
   */
  listOverrides(): Override[] {
    return _.clone(this.overrides);
  }

  /**
   * Set the default baseVersion for the manifest
   */
  setBaseVersion(baseVersion?: string) {
    this.baseVersion = baseVersion;
  }

  /**
   * Find the index to a given override.
   * @returns -1 if it cannot be found
   */
  private findOverrideIndex(overrideName: string): number {
    return this.overrides.findIndex(
      ovr => ovr.name() === normalizePath(overrideName),
    );
  }
}
