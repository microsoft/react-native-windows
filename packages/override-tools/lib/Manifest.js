/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
import * as _ from 'lodash';
import * as crypto from 'crypto';
import * as path from 'path';
export default class Manifest {
    constructor(data, overrideRepo, reactRepo) {
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
    validate() {
        return __awaiter(this, void 0, void 0, function* () {
            const errors = [];
            const manifestFiles = this.overrides.map(override => override.file);
            const overrideFiles = yield this.overrideRepo.listFiles();
            const fileMissingFromManifest = _.difference(overrideFiles, manifestFiles);
            fileMissingFromManifest.forEach(file => errors.push({ type: 'fileMissingFromManifest', file: file }));
            const overridesNotFound = _.difference(manifestFiles, overrideFiles);
            overridesNotFound.forEach(file => errors.push({ type: 'overrideFileNotFound', file: file }));
            yield Promise.all(this.overrides.map((override) => __awaiter(this, void 0, void 0, function* () {
                if (override.type === 'platform') {
                    return;
                }
                const baseFile = override.baseFile;
                const baseContent = yield this.reactRepo.getFileContents(baseFile);
                if (baseContent === null) {
                    errors.push({ type: 'baseFileNotFound', file: override.file });
                    return;
                }
                const baseHash = Manifest.hashContent(baseContent);
                if (baseHash.toLowerCase() !== override.baseHash.toLowerCase()) {
                    errors.push({ type: 'outOfDate', file: override.file });
                    return;
                }
            })));
            return errors;
        });
    }
    /**
     * Add an override to the manifest, throwing if any part is invalid.
     */
    addOverride(type, file, baseFile, issue) {
        return __awaiter(this, void 0, void 0, function* () {
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
        });
    }
    /**
     * Platform override specific version of {@see addOverride}
     */
    addPlatformOverride(file) {
        return __awaiter(this, void 0, void 0, function* () {
            file = yield this.checkAndNormalizeOverrideFile(file);
            this.overrides.push({ type: 'platform', file });
        });
    }
    /**
     * Dervied override specific version of {@see addOverride}
     */
    addDerivedOverride(file, baseFile, issue) {
        return __awaiter(this, void 0, void 0, function* () {
            file = yield this.checkAndNormalizeOverrideFile(file);
            const overrideBaseInfo = yield this.getOverrideBaseInfo(baseFile);
            this.overrides.push(Object.assign(Object.assign({ type: 'derived', file }, overrideBaseInfo), { issue }));
        });
    }
    /**
     * Patch override specific version of {@see addOverride}
     */
    addPatchOverride(file, baseFile, issue) {
        return __awaiter(this, void 0, void 0, function* () {
            file = yield this.checkAndNormalizeOverrideFile(file);
            const overrideBaseInfo = yield this.getOverrideBaseInfo(baseFile);
            this.overrides.push(Object.assign(Object.assign({ type: 'patch', file }, overrideBaseInfo), { issue }));
        });
    }
    /**
     * Copy override specific version of {@see addOverride}
     */
    addCopyOverride(file, baseFile, issue) {
        return __awaiter(this, void 0, void 0, function* () {
            file = yield this.checkAndNormalizeOverrideFile(file);
            const overrideBaseInfo = yield this.getOverrideBaseInfo(baseFile);
            this.overrides.push(Object.assign(Object.assign({ type: 'copy', file }, overrideBaseInfo), { issue }));
        });
    }
    /**
     * Whether the manifest contains a given override
     */
    hasOverride(overridePath) {
        return this.overrides.findIndex(ovr => ovr.file === overridePath) !== -1;
    }
    /**
     * Try to remove an override.
     * @returns false if none is found with the given name
     */
    removeOverride(overridePath) {
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
    findOverride(overridePath) {
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
    markUpToDate(overridePath) {
        return __awaiter(this, void 0, void 0, function* () {
            const entryIdx = this.findOverrideIndex(overridePath);
            if (entryIdx === -1) {
                throw new Error(`Override at '${overridePath}' does not exist`);
            }
            const internalEntry = this.overrides[entryIdx];
            if (internalEntry.type === 'platform') {
                throw new Error('Cannot mark a platform override as up-to-date');
            }
            const newBaseInfo = yield this.getOverrideBaseInfo(internalEntry.baseFile);
            Object.assign(internalEntry, internalEntry, newBaseInfo);
        });
    }
    /**
     * Return a copy of the manifest as raw data
     */
    getAsData() {
        return { overrides: _.cloneDeep(this.overrides) };
    }
    /**
     * Returns the version of React Native the manifest considers as current
     */
    currentVersion() {
        return this.reactRepo.getVersion();
    }
    /**
     * Hash content into the form expected in a manifest entry. Exposed for
     * testing.
     */
    static hashContent(str) {
        const hasher = crypto.createHash('sha1');
        const normalizedStr = str.replace(/(?<!\r)\n/g, '\r\n');
        hasher.update(normalizedStr);
        return hasher.digest('hex');
    }
    /**
     * Find the index to a given override.
     * @returns -1 if it cannot be found
     */
    findOverrideIndex(overridePath) {
        const noramlizedPath = path.normalize(overridePath);
        return this.overrides.findIndex(ovr => ovr.file === noramlizedPath);
    }
    checkAndNormalizeOverrideFile(file) {
        return __awaiter(this, void 0, void 0, function* () {
            const normalizedFile = path.normalize(file);
            if (this.hasOverride(normalizedFile)) {
                throw new Error(`Override '${file}' already exists in manifest`);
            }
            const contents = yield this.overrideRepo.getFileContents(normalizedFile);
            if (contents === null) {
                throw new Error(`Could not find override '${file}'`);
            }
            return normalizedFile;
        });
    }
    getOverrideBaseInfo(file) {
        return __awaiter(this, void 0, void 0, function* () {
            const baseFile = path.normalize(file);
            const baseContent = yield this.reactRepo.getFileContents(baseFile);
            if (baseContent === null) {
                throw new Error(`Could not find base file '${file}'`);
            }
            const baseVersion = this.currentVersion();
            const baseHash = Manifest.hashContent(baseContent);
            return { baseFile, baseVersion, baseHash };
        });
    }
}
//# sourceMappingURL=Manifest.js.map