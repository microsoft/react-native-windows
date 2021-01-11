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
import * as FileRepository from '../FileRepository';
import * as ManifestData from '../ManifestData';
import * as ora from 'ora';
import * as path from 'path';
import GitReactFileRepository from '../GitReactFileRepository';
import Manifest from '../Manifest';
import OverrideFileRepositoryImpl from '../OverrideFileRepositoryImpl';
import { diff_match_patch } from 'diff-match-patch';
import { getInstalledRNVersion } from '../ReactVersion';
const WIN_PLATFORM_EXT = /\.win32|\.windows|\.windesktop/;
const WHITESPACE_PATTERN = /\s/g;
(() => __awaiter(void 0, void 0, void 0, function* () {
    const ovrPath = process.argv[2];
    if (!ovrPath) {
        throw new Error('Expected ovrPath to be provided');
    }
    const spinner = ora();
    spinner.start('Creating manifest');
    const version = yield getInstalledRNVersion(ovrPath);
    const [overrides, reactSources] = yield getFileRepos(ovrPath, version);
    const manifest = { overrides: [] };
    const overrideFiles = yield overrides.listFiles();
    let i = 0;
    for (const file of overrideFiles) {
        spinner.text = `Creating manifest (${++i}/${overrideFiles.length})`;
        const contents = yield overrides.getFileContents(file);
        (yield tryAddCopy(file, version, contents, reactSources, manifest)) ||
            (yield tryAddPatch(file, version, contents, reactSources, manifest)) ||
            (yield tryAddDerived(file, version, contents, reactSources, manifest)) ||
            addUnknown(file, version, manifest);
    }
    const ovrFile = path.join(ovrPath, 'overrides.json');
    yield ManifestData.writeToFile(manifest, ovrFile);
    spinner.succeed();
}))();
function tryAddCopy(filename, rnVersion, override, reactSources, manifest) {
    return __awaiter(this, void 0, void 0, function* () {
        const baseContents = yield reactSources.getFileContents(filename);
        const baseSignificantChars = baseContents.replace(WHITESPACE_PATTERN, '');
        const ovrSignificantChars = override.replace(WHITESPACE_PATTERN, '');
        if (baseSignificantChars !== ovrSignificantChars) {
            return false;
        }
        manifest.overrides.push({
            type: 'copy',
            file: filename,
            baseFile: filename,
            baseVersion: rnVersion,
            baseHash: Manifest.hashContent(baseContents),
            issue: 0,
        });
        return true;
    });
}
function tryAddPatch(filename, rnVersion, override, reactSources, manifest) {
    return __awaiter(this, void 0, void 0, function* () {
        const baseFile = filename.replace(WIN_PLATFORM_EXT, '');
        const baseContents = yield reactSources.getFileContents(baseFile);
        if (!baseContents) {
            return false;
        }
        const { similar } = computeSimilarity(override, baseContents);
        if (similar) {
            manifest.overrides.push({
                type: 'patch',
                file: filename,
                baseFile: baseFile,
                baseVersion: rnVersion,
                baseHash: Manifest.hashContent(baseContents),
                issue: 'LEGACY_FIXME',
            });
        }
        else {
            addUnknown(filename, rnVersion, manifest);
        }
        return true;
    });
}
function tryAddDerived(filename, rnVersion, override, reactSources, manifest) {
    return __awaiter(this, void 0, void 0, function* () {
        const matches = [];
        const droidFile = filename.replace(WIN_PLATFORM_EXT, '.android');
        const droidContents = yield reactSources.getFileContents(droidFile);
        const droidSim = droidContents && computeSimilarity(override, droidContents);
        if (droidSim && droidSim.similar) {
            matches.push({
                file: droidFile,
                contents: droidContents,
                dist: droidSim.editDistance,
            });
        }
        const iosFile = filename.replace(WIN_PLATFORM_EXT, '.ios');
        const iosContents = yield reactSources.getFileContents(iosFile);
        const iosSim = iosContents && computeSimilarity(override, iosContents);
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
            baseHash: Manifest.hashContent(bestMatch.contents),
            issue: 'LEGACY_FIXME',
        });
        return true;
    });
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
function getFileRepos(overrideovrPath, rnVersion) {
    return __awaiter(this, void 0, void 0, function* () {
        const overrides = new OverrideFileRepositoryImpl(overrideovrPath);
        const versionedReactSources = yield GitReactFileRepository.createAndInit();
        const reactSources = FileRepository.bindVersion(versionedReactSources, rnVersion);
        return [overrides, reactSources];
    });
}
function computeSimilarity(override, source) {
    override = stripCopyrightHeaders(override);
    source = stripCopyrightHeaders(source);
    const differ = new diff_match_patch();
    const diffs = differ.diff_main(source, override);
    const editDistance = differ.diff_levenshtein(diffs);
    const similar = editDistance / Math.max(override.length, source.length) < 0.8;
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