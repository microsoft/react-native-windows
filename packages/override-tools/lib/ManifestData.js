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
import * as fs from 'fs';
import * as path from 'path';
import * as t from 'io-ts';
import { ThrowReporter } from 'io-ts/lib/ThrowReporter';
/**
 * Manifest entry type class for "platform" overrides. I.e. overrides not
 * patching shared code, or derived from existing code.
 */
const PlatformEntryType = t.type({
    type: t.literal('platform'),
    file: t.string,
});
/**
 * Manifest entry type class for overrides that patch shared upstream code.
 */
const PatchEntryType = t.type({
    type: t.literal('patch'),
    file: t.string,
    baseFile: t.string,
    baseVersion: t.string,
    baseHash: t.string,
    // Allow LEGACY_FIXME for existing overrides that don't have issues yet
    issue: t.union([t.number, t.literal('LEGACY_FIXME')]),
});
/**
 * Manifest entry type class for overrides that mimic existing upstream code.
 */
const DerivedEntryType = t.type({
    type: t.literal('derived'),
    file: t.string,
    baseFile: t.string,
    baseVersion: t.string,
    baseHash: t.string,
    // Allow LEGACY_FIXME for existing overrides that don't have issues yet
    issue: t.union([t.undefined, t.number, t.literal('LEGACY_FIXME')]),
});
/**
 * Manifest entry type class for direct copies of upstream code
 */
const CopyEntryType = t.type({
    type: t.literal('copy'),
    file: t.string,
    baseFile: t.string,
    baseVersion: t.string,
    baseHash: t.string,
    issue: t.number,
});
const NonPlatformEntryType = t.union([
    PatchEntryType,
    DerivedEntryType,
    CopyEntryType,
]);
const EntryType = t.union([PlatformEntryType, NonPlatformEntryType]);
const ManifestType = t.type({ overrides: t.array(EntryType) });
/**
 * Read an override manifest from a file.
 *
 * @throws if the file is invalid or cannot be found
 */
export function readFromFile(filePath) {
    return __awaiter(this, void 0, void 0, function* () {
        const json = (yield fs.promises.readFile(filePath)).toString();
        return this.parse(json);
    });
}
/**
 * Parse a string with JSON for the override manifest into one.
 *
 * @throws if the JSON doesn't describe a valid manifest
 */
export function parse(json) {
    const parsed = JSON.parse(json);
    ThrowReporter.report(ManifestType.decode(parsed));
    // Make sure directory separators go the right direction
    parsed.overrides.forEach(override => {
        override.file = path.normalize(override.file);
        if (override.type !== 'platform') {
            override.baseFile = path.normalize(override.baseFile);
        }
    });
    return parsed;
}
/**
 * Writes the manifest to a JSON file. Does not validate correctness of the
 * manifest.
 */
export function writeToFile(manifest, filePath) {
    return __awaiter(this, void 0, void 0, function* () {
        manifest.overrides.sort((a, b) => a.file.localeCompare(b.file));
        const json = JSON.stringify(manifest, null /*replacer*/, 2 /*space*/);
        yield fs.promises.mkdir(path.dirname(filePath), { recursive: true });
        yield fs.promises.writeFile(filePath, json);
    });
}
//# sourceMappingURL=ManifestData.js.map