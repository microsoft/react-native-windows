"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    Object.defineProperty(o, k2, { enumerable: true, get: function() { return m[k]; } });
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __exportStar = (this && this.__exportStar) || function(m, exports) {
    for (var p in m) if (p !== "default" && !exports.hasOwnProperty(p)) __createBinding(exports, m, p);
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.upgradeOverrides = exports.addOverride = exports.getOverrideFactory = exports.removeOverride = exports.hasOverride = exports.validateManifest = void 0;
const Serialized = require("./Serialized");
const _ = require("lodash");
const fs = require("fs");
const path = require("path");
const OverrideFactory_1 = require("./OverrideFactory");
const FileRepository_1 = require("./FileRepository");
const async_1 = require("async");
const FileSystemRepository_1 = require("./FileSystemRepository");
const GitReactFileRepository_1 = require("./GitReactFileRepository");
const Manifest_1 = require("./Manifest");
const FileSearch_1 = require("./FileSearch");
const PackageUtils_1 = require("./PackageUtils");
// Re-export types used in the public API so external packages don't have to
// reach into our guts to import them.
__exportStar(require("./OverrideFactory"), exports);
__exportStar(require("./Override"), exports);
const MAX_CONCURRENT_TASKS = 30;
/**
 * Check that the given manifest correctly describe overrides and that all
 * overrides are up to date
 */
async function validateManifest(opts) {
    const { manifest, overrideRepo, reactRepo } = await createManifestContext(opts);
    return await manifest.validate(overrideRepo, reactRepo);
}
exports.validateManifest = validateManifest;
/**
 * Return whether the override exists in the manifest
 */
async function hasOverride(overrideName, opts) {
    const { manifest } = await createManifestContext(opts);
    return manifest.hasOverride(overrideName);
}
exports.hasOverride = hasOverride;
/**
 * Removes an override from the manifest if it exists.
 * @returns whether the override was removed
 */
async function removeOverride(overrideName, opts) {
    const { manifest, manifestPath } = await createManifestContext(opts);
    const removed = manifest.removeOverride(overrideName);
    if (removed) {
        await Serialized.writeManifestToFile(manifest.serialize(), manifestPath);
    }
    return removed;
}
exports.removeOverride = removeOverride;
/**
 * Returns a factory to create overrides which may be added to the manifest
 */
async function getOverrideFactory(opts) {
    return (await createManifestContext(opts)).overrideFactory;
}
exports.getOverrideFactory = getOverrideFactory;
/**
 * Adds an override to the manifest
 */
async function addOverride(override, opts) {
    const { manifest, manifestPath } = await createManifestContext(opts);
    await manifest.addOverride(override);
    await Serialized.writeManifestToFile(manifest.serialize(), manifestPath);
}
exports.addOverride = addOverride;
/**
 * Attempts to automatically merge changes from the current version into
 * out-of-date overrides.
 */
async function upgradeOverrides(opts) {
    const ctx = await createManifestContext(opts);
    const validationErrors = await ctx.manifest.validate(ctx.overrideRepo, ctx.reactRepo);
    const outOfDateOverrides = validationErrors
        .filter(err => err.type === 'outOfDate')
        .map(err => ctx.manifest.findOverride(err.overrideName));
    // Perform upgrades concurrently so we can take advantage of
    // GitReactFileRepository optimizations when multiple requests are queued at
    // once.
    let i = 0;
    const upgradeResults = await async_1.mapLimit(outOfDateOverrides, MAX_CONCURRENT_TASKS, async (override) => {
        const upgradeResult = await override
            .upgradeStrategy()
            .upgrade(ctx.gitReactRepo, ctx.overrideRepo, ctx.reactNativeVersion, opts.allowConflicts);
        if (opts.progressListener) {
            opts.progressListener(++i, outOfDateOverrides.length);
        }
        return upgradeResult;
    });
    // Regenerate overrides that are already up to date to update the baseVersion
    // to current. This helps to minimize the numbers of versions we have to
    // check out for future upgrades.
    const upToDateOverrides = [
        ..._.difference(await ctx.manifest.listOverrides(), validationErrors.map(err => ctx.manifest.findOverride(err.overrideName))).map(ovr => ovr.name()),
        ...upgradeResults
            .filter(res => res.filesWritten)
            .map(res => res.overrideName),
    ];
    await async_1.eachLimit(upToDateOverrides, MAX_CONCURRENT_TASKS, async (name) => {
        await ctx.manifest.markUpToDate(name, ctx.overrideFactory);
    });
    await Serialized.writeManifestToFile(ctx.manifest.serialize(), ctx.manifestPath);
    return upgradeResults.sort((a, b) => a.overrideName.localeCompare(b.overrideName, 'en'));
}
exports.upgradeOverrides = upgradeOverrides;
/**
 * Throw if a file doesn't exist, printing an error message on the way
 */
async function checkFileExists(friendlyName, filePath) {
    try {
        await fs.promises.access(filePath);
    }
    catch (ex) {
        throw new Error(`Could not find ${friendlyName} at path '${filePath}'`);
    }
}
/**
 * Sets up state for a manifest describing overrides at a specified RN version
 */
async function createManifestContext(opts) {
    const manifestPath = opts.manifestPath || (await FileSearch_1.findManifest());
    await checkFileExists('manifest', manifestPath);
    const reactNativeVersion = opts.reactNativeVersion || (await PackageUtils_1.getInstalledRNVersion());
    const overrideDir = path.dirname(manifestPath);
    const overrideRepo = new FileSystemRepository_1.default(overrideDir);
    const gitReactRepo = await GitReactFileRepository_1.default.createAndInit();
    const reactRepo = FileRepository_1.bindVersion(gitReactRepo, reactNativeVersion);
    const overrideFactory = new OverrideFactory_1.OverrideFactoryImpl(reactRepo, overrideRepo);
    const manifest = Manifest_1.default.fromSerialized(await Serialized.readManifestFromFile(manifestPath));
    return {
        overrideRepo,
        reactRepo,
        gitReactRepo,
        overrideFactory,
        manifestPath,
        manifest,
        reactNativeVersion,
    };
}
//# sourceMappingURL=Api.js.map