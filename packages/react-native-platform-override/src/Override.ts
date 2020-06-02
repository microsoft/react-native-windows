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

export default interface Override {
  name(): string;
  includesFile(filename: string): boolean;
  serialize(): Serialized.Override;
  createUpdated(factory: OverrideFactory): Promise<Override>;
  upgradeStrategy(): UpgradeStrategy;
  validationStrategies(): ValidationStrategy[];
}

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
    return filename === this.overrideFile;
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
    return filename === this.overrideFile;
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
      issue: this.issueNumber as number,
      ...this.serialzeBase(),
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
      issue: this.issueNumber || undefined,
      ...this.serialzeBase(),
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
      issue: this.issueNumber as number,
      ...this.serialzeBase(),
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
