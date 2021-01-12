"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const FileRepository = require("../FileRepository");
const Serialized = require("../Serialized");
const ora = require("ora");
const path = require("path");
const FileSystemRepository_1 = require("../FileSystemRepository");
const GitReactFileRepository_1 = require("../GitReactFileRepository");
const diff_match_patch_1 = require("diff-match-patch");
const PackageUtils_1 = require("../PackageUtils");
const Hash_1 = require("../Hash");
const isutf8_1 = require("isutf8");
const WIN_PLATFORM_EXT = /\.win32|\.windows|\.windesktop/;
(async () => {
    const ovrPath = process.argv[2];
    if (!ovrPath) {
        throw new Error('Expected ovrPath to be provided');
    }
    const spinner = ora();
    spinner.start('Creating manifest');
    const version = await PackageUtils_1.getInstalledRNVersion();
    const [overrides, reactSources] = await getFileRepos(ovrPath, version);
    const manifest = {
        includePatterns: undefined,
        excludePatterns: undefined,
        overrides: [],
    };
    const overrideFiles = await overrides.listFiles();
    let i = 0;
    for (const file of overrideFiles) {
        spinner.text = `Creating manifest (${++i}/${overrideFiles.length})`;
        const contents = (await overrides.readFile(file));
        (await tryAddCopy(file, version, contents, reactSources, manifest)) ||
            (await tryAddPatch(file, version, contents, reactSources, manifest)) ||
            (await tryAddDerived(file, version, contents, reactSources, manifest)) ||
            addUnknown(file, version, manifest);
    }
    const ovrFile = path.join(ovrPath, 'overrides.json');
    await Serialized.writeManifestToFile(manifest, ovrFile);
    spinner.succeed();
})();
async function tryAddCopy(filename, rnVersion, overrideContent, reactSources, manifest) {
    const baseContent = await reactSources.readFile(filename);
    if (!baseContent) {
        return false;
    }
    if (Hash_1.hashContent(baseContent) !== Hash_1.hashContent(overrideContent)) {
        return false;
    }
    manifest.overrides.push({
        type: 'copy',
        file: filename,
        baseFile: filename,
        baseVersion: rnVersion,
        baseHash: Hash_1.hashContent(baseContent),
        issue: 0,
    });
    return true;
}
async function tryAddPatch(filename, rnVersion, overrideContent, reactSources, manifest) {
    const baseFile = filename.replace(WIN_PLATFORM_EXT, '');
    const baseContent = await reactSources.readFile(baseFile);
    if (!baseContent) {
        return false;
    }
    const { similar } = computeSimilarity(overrideContent, baseContent);
    if (similar) {
        manifest.overrides.push({
            type: 'patch',
            file: filename,
            baseFile: baseFile,
            baseVersion: rnVersion,
            baseHash: Hash_1.hashContent(baseContent),
            issue: 'LEGACY_FIXME',
        });
    }
    else {
        addUnknown(filename, rnVersion, manifest);
    }
    return true;
}
async function tryAddDerived(filename, rnVersion, overrideContent, reactSources, manifest) {
    const matches = [];
    const droidFile = filename.replace(WIN_PLATFORM_EXT, '.android');
    const droidContents = await reactSources.readFile(droidFile);
    const droidSim = droidContents && computeSimilarity(overrideContent, droidContents);
    if (droidSim && droidSim.similar) {
        matches.push({
            file: droidFile,
            contents: droidContents,
            dist: droidSim.editDistance,
        });
    }
    const iosFile = filename.replace(WIN_PLATFORM_EXT, '.ios');
    const iosContents = await reactSources.readFile(iosFile);
    const iosSim = iosContents && computeSimilarity(overrideContent, iosContents);
    if (iosSim && iosSim.similar) {
        matches.push({
            file: iosFile,
            contents: iosContents,
            dist: iosSim.editDistance,
        });
    }
    if (matches.length === 0) {
        return false;
    }
    const bestMatch = matches.reduce((a, b) => (a.dist < b.dist ? a : b));
    manifest.overrides.push({
        type: 'derived',
        file: filename,
        baseFile: bestMatch.file,
        baseVersion: rnVersion,
        baseHash: Hash_1.hashContent(bestMatch.contents),
        issue: 'LEGACY_FIXME',
    });
    return true;
}
function addUnknown(filename, rnVersion, manifest) {
    manifest.overrides.push({
        type: '???',
        file: filename,
        baseFile: '???',
        baseVersion: rnVersion,
        baseHash: '???',
        issue: 'LEGACY_FIXME',
    });
}
async function getFileRepos(overrideovrPath, rnVersion) {
    const overrides = new FileSystemRepository_1.default(overrideovrPath);
    const versionedReactSources = await GitReactFileRepository_1.default.createAndInit();
    const reactSources = FileRepository.bindVersion(versionedReactSources, rnVersion);
    return [overrides, reactSources];
}
function computeSimilarity(override, source) {
    if (!isutf8_1.default(override) || !isutf8_1.default(source)) {
        return { similar: false, editDistance: NaN };
    }
    let overrideStr = override.toString();
    let sourceStr = source.toString();
    overrideStr = stripCopyrightHeaders(overrideStr);
    sourceStr = stripCopyrightHeaders(sourceStr);
    const differ = new diff_match_patch_1.diff_match_patch();
    const diffs = differ.diff_main(sourceStr, overrideStr);
    const editDistance = differ.diff_levenshtein(diffs);
    const similar = editDistance / Math.max(overrideStr.length, sourceStr.length) < 0.8;
    return { similar, editDistance };
}
function stripCopyrightHeaders(str) {
    if (!str.startsWith('/*')) {
        return str;
    }
    const headerEnd = str.indexOf('*/') + 1;
    return str.slice(headerEnd);
}
//# sourceMappingURL=generateManifest.js.map