"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const _ = require("lodash");
const Override_1 = require("./Override");
const async_1 = require("async");
/**
 * Represents a collection of overrides listed in an on-disk manifest. Allows
 * performing aggregate operations on the overrides.
 */
class Manifest {
    /**
     * Construct the manifest
     *
     * @param overrides List of overrides to evaluate
     * @param opts Allows specifying globs to include or exclude paths to enforce
     * exist in the manifest
     */
    constructor(overrides, opts = {}) {
        const uniquelyNamed = _.uniqBy(overrides, ovr => ovr.name());
        if (uniquelyNamed.length !== overrides.length) {
            throw new Error('Cannot construct a manifest with duplicate overrides');
        }
        this.includePatterns = opts.includePatterns;
        this.excludePatterns = opts.excludePatterns;
        this.overrides = _.clone(overrides);
    }
    static fromSerialized(man) {
        const overrides = man.overrides.map(Override_1.deserializeOverride);
        return new Manifest(overrides, {
            includePatterns: man.includePatterns,
            excludePatterns: man.excludePatterns,
        });
    }
    /**
     * Check that overrides are accurately accounted for in the manifest. E.g.
     * all files should be accounted for, and base files should be up to date
     * with upstream.
     */
    async validate(overrideRepo, reactRepo) {
        const errors = [];
        const globs = [
            ...(this.includePatterns || ['**']),
            ...(this.excludePatterns || []).map(p => '!' + p),
        ];
        const overrideFiles = await overrideRepo.listFiles(globs);
        const missingFromManifest = overrideFiles.filter(file => file !== 'overrides.json' &&
            !this.overrides.some(override => override.includesFile(file)));
        for (const missingFile of missingFromManifest) {
            errors.push({ type: 'missingFromManifest', overrideName: missingFile });
        }
        const validationTasks = _.flatMap(this.overrides, ovr => ovr.validationStrategies());
        await async_1.eachLimit(validationTasks, 30, async (task) => {
            errors.push(...(await task.validate(overrideRepo, reactRepo)));
        });
        return errors.sort((a, b) => a.overrideName.localeCompare(b.overrideName, 'en'));
    }
    /**
     * Add an override to the manifest
     */
    addOverride(override) {
        if (this.hasOverride(override.name())) {
            throw new Error(`Trying to add duplicate override '${override.name()}'`);
        }
        this.overrides.push(override);
    }
    /**
     * Whether the manifest contains a given override
     */
    hasOverride(overrideName) {
        return this.overrides.some(ovr => ovr.name() === overrideName);
    }
    /**
     * Try to remove an override.
     * @returns false if none is found with the given name
     */
    removeOverride(overrideName) {
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
    findOverride(overrideName) {
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
    async markUpToDate(overrideName, overrideFactory) {
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
    serialize() {
        return {
            includePatterns: this.includePatterns,
            excludePatterns: this.excludePatterns,
            overrides: this.overrides
                .sort((a, b) => a.name().localeCompare(b.name(), 'en'))
                .map(override => override.serialize()),
        };
    }
    /**
     * Returns the overrides in the manfest
     */
    listOverrides() {
        return _.clone(this.overrides);
    }
    /**
     * Find the index to a given override.
     * @returns -1 if it cannot be found
     */
    findOverrideIndex(overrideName) {
        return this.overrides.findIndex(ovr => ovr.name() === overrideName);
    }
}
exports.default = Manifest;
//# sourceMappingURL=Manifest.js.map