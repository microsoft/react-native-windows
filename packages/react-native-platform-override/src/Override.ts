/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Serialized from './Serialized';
import * as path from 'path';

import UpgradeStrategy, {UpgradeStrategies} from './UpgradeStrategy';
import ValidationStrategy, {ValidationStrategies} from './ValidationStrategy';
import OverrideFactory from './OverrideFactory';

/**
 * Immmutable programatic representation of an override. This should remain
 * generic to files vs directories, different representations, different
 * validation rules, etc.
 */
export default interface Override {
  /**
   * Identifer of the override (e.g. filename or directory name)
   */
  name(): string;

  /**
   * Does the override include the given file?
   */
  includesFile(filename: string): boolean;

  /**
   * Convert to a serialized representation
   */
  serialize(): Serialized.Override;

  /**
   * Create a copy of the override which is considered "up to date" in regards
   * to the current React source tree. This does not change underlying content.
   */
  createUpdated(factory: OverrideFactory): Promise<Override>;

  /**
   * Specifies how the override should be modified to integrate new changes.
   */
  upgradeStrategy(): UpgradeStrategy;

  /**
   * Specifies how to check if the override contents are valid and up to date.
   */
  validationStrategies(): ValidationStrategy[];
}

/**
 * Platform overrides represent logic not derived from upstream sources.
 */
export class PlatformOverride implements Override {
  private overrideFile: string;

  constructor(args: {file: string}) {
    this.overrideFile = path.normalize(args.file);
  }

  static fromSerialized(
    serialized: Serialized.PlatformOverride,
  ): PlatformOverride {
    return new PlatformOverride(serialized);
  }

  serialize(): Serialized.PlatformOverride {
    return {type: 'platform', file: this.overrideFile};
  }

  name(): string {
    return this.overrideFile;
  }

  includesFile(filename: string): boolean {
    return path.normalize(filename) === this.overrideFile;
  }

  async createUpdated(factory: OverrideFactory): Promise<Override> {
    return factory.createPlatformOverride(this.overrideFile);
  }

  upgradeStrategy(): UpgradeStrategy {
    return UpgradeStrategies.assumeUpToDate();
  }

  validationStrategies(): ValidationStrategy[] {
    return [ValidationStrategies.overrideFileExists(this.overrideFile)];
  }
}

/**
 * Base class for overrides which derive from an upstream file
 */
abstract class BaseFileOverride implements Override {
  protected overrideFile: string;
  protected baseFile: string;
  protected baseVersion: string;
  protected baseHash: string;
  protected issueNumber: number | null | 'LEGACY_FIXME';

  constructor(args: {
    file: string;
    baseFile: string;
    baseVersion: string;
    baseHash: string;
    issue?: number | 'LEGACY_FIXME';
  }) {
    this.overrideFile = path.normalize(args.file);
    this.baseFile = path.normalize(args.baseFile);
    this.baseVersion = args.baseVersion;
    this.baseHash = args.baseHash;
    this.issueNumber = args.issue || null;
  }

  name(): string {
    return this.overrideFile;
  }

  includesFile(filename: string): boolean {
    return path.normalize(filename) === this.overrideFile;
  }

  abstract serialize(): Serialized.Override;
  abstract createUpdated(factory: OverrideFactory): Promise<Override>;
  abstract upgradeStrategy(): UpgradeStrategy;

  validationStrategies(): ValidationStrategy[] {
    return [
      ValidationStrategies.baseFileExists(this.overrideFile, this.baseFile),
      ValidationStrategies.overrideFileExists(this.overrideFile),
      ValidationStrategies.baseFileUpToDate(
        this.overrideFile,
        this.baseFile,
        this.baseHash,
      ),
    ];
  }

  protected serialzeBase() {
    return {
      file: this.overrideFile,
      baseFile: this.baseFile,
      baseVersion: this.baseVersion,
      baseHash: this.baseHash,
    };
  }
}

/**
 * Copy overrides enforce that an override file is an exact copy of a base file
 */
export class CopyOverride extends BaseFileOverride {
  constructor(args: {
    file: string;
    baseFile: string;
    baseVersion: string;
    baseHash: string;
    issue: number;
  }) {
    super(args);
  }

  static fromSerialized(serialized: Serialized.CopyOverride): CopyOverride {
    return new CopyOverride(serialized);
  }

  serialize(): Serialized.CopyOverride {
    return {
      type: 'copy',
      ...this.serialzeBase(),
      issue: this.issueNumber as number,
    };
  }

  async createUpdated(factory: OverrideFactory): Promise<Override> {
    return factory.createCopyOverride(this.overrideFile, this.baseFile, this
      .issueNumber as number);
  }

  upgradeStrategy(): UpgradeStrategy {
    return UpgradeStrategies.copyFile(this.overrideFile, this.baseFile);
  }

  validationStrategies(): ValidationStrategy[] {
    return [
      ...super.validationStrategies(),
      ValidationStrategies.overrideCopyOfBaseFile(
        this.overrideFile,
        this.baseFile,
      ),
    ];
  }
}

/**
 * Derived overrides represent files which are based off of an existing file in
 * the React Native source tree.
 */
export class DerivedOverride extends BaseFileOverride {
  constructor(args: {
    file: string;
    baseFile: string;
    baseVersion: string;
    baseHash: string;
    issue?: number | 'LEGACY_FIXME';
  }) {
    super(args);
  }

  static fromSerialized(
    serialized: Serialized.DerivedOverride,
  ): DerivedOverride {
    return new DerivedOverride(serialized);
  }

  serialize(): Serialized.DerivedOverride {
    return {
      type: 'derived',
      ...this.serialzeBase(),
      issue: this.issueNumber || undefined,
    };
  }

  async createUpdated(factory: OverrideFactory): Promise<Override> {
    return factory.createDerivedOverride(
      this.overrideFile,
      this.baseFile,
      this.issueNumber || undefined,
    );
  }

  upgradeStrategy(): UpgradeStrategy {
    return UpgradeStrategies.threeWayMerge(
      this.overrideFile,
      this.baseFile,
      this.baseVersion,
    );
  }
}

/**
 * Patch overrides represent files which make minor modifications to existing
 * upstream sources.
 */
export class PatchOverride extends BaseFileOverride {
  constructor(args: {
    file: string;
    baseFile: string;
    baseVersion: string;
    baseHash: string;
    issue?: number | 'LEGACY_FIXME';
  }) {
    super(args);
  }

  static fromSerialized(serialized: Serialized.PatchOverride): PatchOverride {
    return new PatchOverride(serialized);
  }

  serialize(): Serialized.PatchOverride {
    return {
      type: 'patch',
      ...this.serialzeBase(),
      issue: this.issueNumber as number,
    };
  }

  async createUpdated(factory: OverrideFactory): Promise<Override> {
    return factory.createPatchOverride(
      this.overrideFile,
      this.baseFile,
      this.issueNumber!,
    );
  }

  upgradeStrategy(): UpgradeStrategy {
    return UpgradeStrategies.threeWayMerge(
      this.overrideFile,
      this.baseFile,
      this.baseVersion,
    );
  }
}

export function deserializeOverride(ovr: Serialized.Override): Override {
  switch (ovr.type) {
    case 'platform':
      return PlatformOverride.fromSerialized(ovr);
    case 'copy':
      return CopyOverride.fromSerialized(ovr);
    case 'derived':
      return DerivedOverride.fromSerialized(ovr);
    case 'patch':
      return PatchOverride.fromSerialized(ovr);
  }
}
