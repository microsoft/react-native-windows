"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.deserializeOverride = exports.DirectoryCopyOverride = exports.PatchOverride = exports.DerivedOverride = exports.CopyOverride = exports.PlatformOverride = void 0;
const path = require("path");
const UpgradeStrategy_1 = require("./UpgradeStrategy");
const ValidationStrategy_1 = require("./ValidationStrategy");
/**
 * Platform overrides represent logic not derived from upstream sources.
 */
class PlatformOverride {
    constructor(args) {
        this.overrideFile = path.normalize(args.file);
    }
    static fromSerialized(serialized) {
        return new PlatformOverride(serialized);
    }
    serialize() {
        return { type: 'platform', file: unixPath(this.overrideFile) };
    }
    name() {
        return this.overrideFile;
    }
    includesFile(filename) {
        return path.normalize(filename) === this.overrideFile;
    }
    async createUpdated(factory) {
        return factory.createPlatformOverride(this.overrideFile);
    }
    upgradeStrategy() {
        return UpgradeStrategy_1.UpgradeStrategies.assumeUpToDate(this.overrideFile);
    }
    validationStrategies() {
        return [ValidationStrategy_1.ValidationStrategies.overrideFileExists(this.overrideFile)];
    }
}
exports.PlatformOverride = PlatformOverride;
/**
 * Base class for overrides which derive from an upstream file
 */
class BaseFileOverride {
    constructor(args) {
        this.overrideFile = path.normalize(args.file);
        this.baseFile = path.normalize(args.baseFile);
        this.baseVersion = args.baseVersion;
        this.baseHash = args.baseHash;
        this.issueNumber = args.issue || null;
    }
    name() {
        return this.overrideFile;
    }
    includesFile(filename) {
        return path.normalize(filename) === this.overrideFile;
    }
    validationStrategies() {
        return [
            ValidationStrategy_1.ValidationStrategies.baseFileExists(this.overrideFile, this.baseFile),
            ValidationStrategy_1.ValidationStrategies.overrideFileExists(this.overrideFile),
            ValidationStrategy_1.ValidationStrategies.baseUpToDate(this.overrideFile, this.baseFile, this.baseHash),
        ];
    }
    serialzeBase() {
        return {
            file: unixPath(this.overrideFile),
            baseFile: unixPath(this.baseFile),
            baseVersion: this.baseVersion,
            baseHash: this.baseHash,
        };
    }
}
/**
 * Copy overrides enforce that an override file is an exact copy of a base file
 */
class CopyOverride extends BaseFileOverride {
    constructor(args) {
        super(args);
    }
    static fromSerialized(serialized) {
        return new CopyOverride(serialized);
    }
    serialize() {
        return Object.assign(Object.assign({ type: 'copy' }, this.serialzeBase()), { issue: this.issueNumber });
    }
    async createUpdated(factory) {
        return factory.createCopyOverride(this.overrideFile, this.baseFile, this.issueNumber);
    }
    upgradeStrategy() {
        return UpgradeStrategy_1.UpgradeStrategies.copyFile(this.overrideFile, this.baseFile);
    }
    validationStrategies() {
        return [
            ...super.validationStrategies(),
            ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase(this.overrideFile, this.baseFile),
        ];
    }
}
exports.CopyOverride = CopyOverride;
/**
 * Derived overrides represent files which are based off of an existing file in
 * the React Native source tree.
 */
class DerivedOverride extends BaseFileOverride {
    constructor(args) {
        super(args);
    }
    static fromSerialized(serialized) {
        return new DerivedOverride(serialized);
    }
    serialize() {
        return Object.assign(Object.assign({ type: 'derived' }, this.serialzeBase()), { issue: this.issueNumber || undefined });
    }
    async createUpdated(factory) {
        return factory.createDerivedOverride(this.overrideFile, this.baseFile, this.issueNumber || undefined);
    }
    upgradeStrategy() {
        return UpgradeStrategy_1.UpgradeStrategies.threeWayMerge(this.overrideFile, this.baseFile, this.baseVersion);
    }
    validationStrategies() {
        return [
            ...super.validationStrategies(),
            ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase(this.overrideFile, this.baseFile),
        ];
    }
}
exports.DerivedOverride = DerivedOverride;
/**
 * Patch overrides represent files which make minor modifications to existing
 * upstream sources.
 */
class PatchOverride extends BaseFileOverride {
    constructor(args) {
        super(args);
    }
    static fromSerialized(serialized) {
        return new PatchOverride(serialized);
    }
    serialize() {
        return Object.assign(Object.assign({ type: 'patch' }, this.serialzeBase()), { issue: this.issueNumber });
    }
    async createUpdated(factory) {
        return factory.createPatchOverride(this.overrideFile, this.baseFile, this.issueNumber);
    }
    upgradeStrategy() {
        return UpgradeStrategy_1.UpgradeStrategies.threeWayMerge(this.overrideFile, this.baseFile, this.baseVersion);
    }
    validationStrategies() {
        return [
            ...super.validationStrategies(),
            ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase(this.overrideFile, this.baseFile),
        ];
    }
}
exports.PatchOverride = PatchOverride;
/**
 * DirectoryCopy overrides copy files from an upstream directory
 */
class DirectoryCopyOverride {
    constructor(args) {
        this.diretory = path.normalize(args.directory);
        this.baseDirectory = path.normalize(args.baseDirectory);
        this.baseVersion = args.baseVersion;
        this.baseHash = args.baseHash;
        this.issue = args.issue;
    }
    static fromSerialized(serialized) {
        return new DirectoryCopyOverride(serialized);
    }
    serialize() {
        return {
            type: 'copy',
            directory: unixPath(this.diretory),
            baseDirectory: unixPath(this.baseDirectory),
            baseVersion: this.baseVersion,
            baseHash: this.baseHash,
            issue: this.issue,
        };
    }
    name() {
        return this.diretory;
    }
    includesFile(filename) {
        const relativeToDir = path.relative(this.diretory, path.normalize(filename));
        return relativeToDir.split(path.sep)[0] !== '..';
    }
    async createUpdated(factory) {
        return factory.createDirectoryCopyOverride(this.diretory, this.baseDirectory, this.issue);
    }
    upgradeStrategy() {
        return UpgradeStrategy_1.UpgradeStrategies.copyDirectory(this.diretory, this.baseDirectory);
    }
    validationStrategies() {
        return [
            ValidationStrategy_1.ValidationStrategies.overrideDirectoryExists(this.diretory),
            ValidationStrategy_1.ValidationStrategies.baseDirectoryExists(this.diretory, this.baseDirectory),
            ValidationStrategy_1.ValidationStrategies.baseUpToDate(this.diretory, this.baseDirectory, this.baseHash),
            ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase(this.diretory, this.baseDirectory),
        ];
    }
}
exports.DirectoryCopyOverride = DirectoryCopyOverride;
function deserializeOverride(ovr) {
    switch (ovr.type) {
        case 'platform':
            return PlatformOverride.fromSerialized(ovr);
        case 'copy':
            return 'directory' in ovr
                ? DirectoryCopyOverride.fromSerialized(ovr)
                : CopyOverride.fromSerialized(ovr);
        case 'derived':
            return DerivedOverride.fromSerialized(ovr);
        case 'patch':
            return PatchOverride.fromSerialized(ovr);
    }
}
exports.deserializeOverride = deserializeOverride;
function unixPath(dir) {
    return dir.replace(/\\/g, '/');
}
//# sourceMappingURL=Override.js.map