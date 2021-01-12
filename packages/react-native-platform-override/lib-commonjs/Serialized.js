"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.writeManifestToFile = exports.parseManifest = exports.readManifestFromFile = void 0;
const fs = require("fs");
const path = require("path");
const t = require("io-ts");
const ThrowReporter_1 = require("io-ts/lib/ThrowReporter");
/**
 * Serialized form of {@see PlatformOverride}
 */
const PlatformOverrideType = t.type({
    type: t.literal('platform'),
    file: t.string,
});
/**
 * Serialized form of {@see PatchOverride}
 */
const PatchOverrideType = t.type({
    type: t.literal('patch'),
    file: t.string,
    baseFile: t.string,
    baseVersion: t.string,
    baseHash: t.string,
    // Allow LEGACY_FIXME for existing overrides that don't have issues yet
    issue: t.union([t.number, t.literal('LEGACY_FIXME')]),
});
/**
 * Serialized form of {@see DerivedOverride}
 */
const DerivedOverrideType = t.type({
    type: t.literal('derived'),
    file: t.string,
    baseFile: t.string,
    baseVersion: t.string,
    baseHash: t.string,
    // Allow LEGACY_FIXME for existing overrides that don't have issues yet
    issue: t.union([t.undefined, t.number, t.literal('LEGACY_FIXME')]),
});
/**
 * Serialized form of {@see CopyOverride}
 */
const CopyOverrideType = t.type({
    type: t.literal('copy'),
    file: t.string,
    baseFile: t.string,
    baseVersion: t.string,
    baseHash: t.string,
    issue: t.number,
});
/**
 * Serialized form of {@see DirectoryCopyOverride}
 */
const DirectoryCopyOverrideType = t.type({
    type: t.literal('copy'),
    directory: t.string,
    baseDirectory: t.string,
    baseVersion: t.string,
    baseHash: t.string,
    issue: t.number,
});
/**
 * Union of all serialized override types
 */
const OverrideType = t.union([
    PlatformOverrideType,
    PatchOverrideType,
    DerivedOverrideType,
    CopyOverrideType,
    DirectoryCopyOverrideType,
]);
/**
 * Schema for the "overrides.json" manifest
 */
const ManifestType = t.type({
    includePatterns: t.union([t.undefined, t.array(t.string)]),
    excludePatterns: t.union([t.undefined, t.array(t.string)]),
    overrides: t.array(OverrideType),
});
/**
 * Read an override manifest from a file.
 *
 * @throws if the file is invalid or cannot be found
 */
async function readManifestFromFile(filePath) {
    const json = (await fs.promises.readFile(filePath)).toString();
    return parseManifest(json);
}
exports.readManifestFromFile = readManifestFromFile;
/**
 * Parse a string with JSON for the override manifest into one.
 *
 * @throws if the JSON doesn't describe a valid manifest
 */
function parseManifest(json) {
    const parsed = JSON.parse(json);
    try {
        ThrowReporter_1.ThrowReporter.report(ManifestType.decode(parsed));
    }
    catch (ex) {
        throw new Error('Could not parse manifest. Is it valid?');
    }
    return parsed;
}
exports.parseManifest = parseManifest;
/**
 * Writes the manifest to a JSON file. Does not validate correctness of the
 * manifest.
 */
async function writeManifestToFile(manifest, filePath) {
    const json = JSON.stringify(manifest, null /*replacer*/, 2 /*space*/);
    await fs.promises.mkdir(path.dirname(filePath), { recursive: true });
    await fs.promises.writeFile(filePath, json);
}
exports.writeManifestToFile = writeManifestToFile;
//# sourceMappingURL=Serialized.js.map