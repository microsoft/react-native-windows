"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.OverrideFactoryImpl = void 0;
const path = require("path");
const Override_1 = require("./Override");
const Hash_1 = require("./Hash");
/**
 * Conrete implementation of an OverrideFactory
 */
class OverrideFactoryImpl {
    constructor(reactRepo, overrideRepo) {
        this.reactRepo = reactRepo;
        this.overrideRepo = overrideRepo;
    }
    async createPlatformOverride(file) {
        await this.checkOverrideExists(file, 'file');
        return new Override_1.PlatformOverride({ file });
    }
    async createCopyOverride(file, baseFile, issue) {
        await this.checkOverrideExists(file, 'file');
        return new Override_1.CopyOverride(Object.assign({ file,
            baseFile,
            issue }, (await this.getBaseInfo(baseFile))));
    }
    async createDerivedOverride(file, baseFile, issue) {
        await this.checkOverrideExists(file, 'file');
        return new Override_1.DerivedOverride(Object.assign({ file,
            baseFile,
            issue }, (await this.getBaseInfo(baseFile))));
    }
    async createPatchOverride(file, baseFile, issue) {
        await this.checkOverrideExists(file, 'file');
        return new Override_1.PatchOverride(Object.assign({ file,
            baseFile,
            issue }, (await this.getBaseInfo(baseFile))));
    }
    async createDirectoryCopyOverride(directory, baseDirectory, issue) {
        await this.checkOverrideExists(directory, 'directory');
        return new Override_1.DirectoryCopyOverride(Object.assign({ directory,
            baseDirectory,
            issue }, (await this.getBaseInfo(baseDirectory))));
    }
    async checkOverrideExists(overridePath, type) {
        if (path.isAbsolute(overridePath)) {
            throw new Error(`Expected override path to be repo relative. Got '${overridePath}'`);
        }
        if ((await this.overrideRepo.stat(overridePath)) !== type) {
            throw new Error(`Could not find ${type} at repo relative path '${overridePath}'`);
        }
    }
    async getBaseInfo(basePath) {
        if (path.isAbsolute(basePath)) {
            throw new Error(`Expected base path to be repo relative. Got '${basePath}'`);
        }
        const baseType = await this.reactRepo.stat(basePath);
        if (baseType === 'none') {
            throw new Error(`Could not find base file/directory '${basePath}'`);
        }
        const baseVersion = this.reactRepo.getVersion();
        const baseHash = (await Hash_1.hashFileOrDirectory(basePath, this.reactRepo));
        return { baseVersion, baseHash };
    }
}
exports.OverrideFactoryImpl = OverrideFactoryImpl;
//# sourceMappingURL=OverrideFactory.js.map