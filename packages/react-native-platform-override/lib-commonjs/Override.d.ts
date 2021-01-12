/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as Serialized from './Serialized';
import UpgradeStrategy from './UpgradeStrategy';
import ValidationStrategy from './ValidationStrategy';
import OverrideFactory from './OverrideFactory';
/**
 * Immmutable programatic representation of an override. This should remain
 * generic to files vs directories, different representations, different
 * validation rules, etc.
 */
export default interface Override {
    /**
     * Case sensitive identifer of the override (e.g. filename or directory name)
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
export declare class PlatformOverride implements Override {
    private overrideFile;
    constructor(args: {
        file: string;
    });
    static fromSerialized(serialized: Serialized.PlatformOverride): PlatformOverride;
    serialize(): Serialized.PlatformOverride;
    name(): string;
    includesFile(filename: string): boolean;
    createUpdated(factory: OverrideFactory): Promise<Override>;
    upgradeStrategy(): UpgradeStrategy;
    validationStrategies(): ValidationStrategy[];
}
/**
 * Base class for overrides which derive from an upstream file
 */
declare abstract class BaseFileOverride implements Override {
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
    });
    name(): string;
    includesFile(filename: string): boolean;
    abstract serialize(): Serialized.Override;
    abstract createUpdated(factory: OverrideFactory): Promise<Override>;
    abstract upgradeStrategy(): UpgradeStrategy;
    validationStrategies(): ValidationStrategy[];
    protected serialzeBase(): {
        file: string;
        baseFile: string;
        baseVersion: string;
        baseHash: string;
    };
}
/**
 * Copy overrides enforce that an override file is an exact copy of a base file
 */
export declare class CopyOverride extends BaseFileOverride {
    constructor(args: {
        file: string;
        baseFile: string;
        baseVersion: string;
        baseHash: string;
        issue: number;
    });
    static fromSerialized(serialized: Serialized.CopyOverride): CopyOverride;
    serialize(): Serialized.CopyOverride;
    createUpdated(factory: OverrideFactory): Promise<Override>;
    upgradeStrategy(): UpgradeStrategy;
    validationStrategies(): ValidationStrategy[];
}
/**
 * Derived overrides represent files which are based off of an existing file in
 * the React Native source tree.
 */
export declare class DerivedOverride extends BaseFileOverride {
    constructor(args: {
        file: string;
        baseFile: string;
        baseVersion: string;
        baseHash: string;
        issue?: number | 'LEGACY_FIXME';
    });
    static fromSerialized(serialized: Serialized.DerivedOverride): DerivedOverride;
    serialize(): Serialized.DerivedOverride;
    createUpdated(factory: OverrideFactory): Promise<Override>;
    upgradeStrategy(): UpgradeStrategy;
    validationStrategies(): ValidationStrategy[];
}
/**
 * Patch overrides represent files which make minor modifications to existing
 * upstream sources.
 */
export declare class PatchOverride extends BaseFileOverride {
    constructor(args: {
        file: string;
        baseFile: string;
        baseVersion: string;
        baseHash: string;
        issue?: number | 'LEGACY_FIXME';
    });
    static fromSerialized(serialized: Serialized.PatchOverride): PatchOverride;
    serialize(): Serialized.PatchOverride;
    createUpdated(factory: OverrideFactory): Promise<Override>;
    upgradeStrategy(): UpgradeStrategy;
    validationStrategies(): ValidationStrategy[];
}
/**
 * DirectoryCopy overrides copy files from an upstream directory
 */
export declare class DirectoryCopyOverride implements Override {
    private diretory;
    private baseDirectory;
    private baseVersion;
    private baseHash;
    private issue;
    constructor(args: {
        directory: string;
        baseDirectory: string;
        baseVersion: string;
        baseHash: string;
        issue: number;
    });
    static fromSerialized(serialized: Serialized.DirectoryCopyOverride): DirectoryCopyOverride;
    serialize(): Serialized.DirectoryCopyOverride;
    name(): string;
    includesFile(filename: string): boolean;
    createUpdated(factory: OverrideFactory): Promise<Override>;
    upgradeStrategy(): UpgradeStrategy;
    validationStrategies(): ValidationStrategy[];
}
export declare function deserializeOverride(ovr: Serialized.Override): Override;
export {};
