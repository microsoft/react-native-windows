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
import Manifest from '../Manifest';
import { MockOverrideFileRepository, MockReactFileRepository, } from './MockFileRepository';
const reactFiles = [
    {
        filename: 'aaa\\aaa.js',
        content: 'I want your love, and I want your revenge;You and me could write a bad romance',
    },
    {
        filename: 'aaa\\bbb.android.js',
        content: "Gimme, gimme, gimme a man after midnight. Won't somebody help me chase the shadows away",
    },
    {
        filename: 'bbb\\ccc.ios.js',
        content: "Cause honey I'll come get my things, but I can't let go I'm waiting for it, that green light, I want it",
    },
];
const overrideFiles = [
    {
        filename: 'aaa\\aaa.windows.js',
        content: 'I want your love, and I want your mashed potatoes;You and me could make more mashed potatoes I guess',
    },
    {
        filename: 'aaa\\bbb.windows.js',
        content: "Gimme, gimme, gimme 500 live bees after midnight. Won't somebody help me chase the bees out of my house, this is actually quite frightening",
    },
    {
        filename: 'bbb\\ccc.win32.js',
        content: "Cause honey I'll come get my things, but I can't let go I'm waiting for it, the fall of civilization as we know it",
    },
];
const reactRepo = new MockReactFileRepository(reactFiles);
const ovrRepo = new MockOverrideFileRepository(overrideFiles);
test('AllListedInManifest', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = {
        overrides: [
            { type: 'platform', file: 'aaa\\aaa.windows.js' },
            { type: 'platform', file: 'aaa\\bbb.windows.js' },
            { type: 'platform', file: 'bbb\\ccc.win32.js' },
        ],
    };
    const errors = yield new Manifest(manifest, ovrRepo, reactRepo).validate();
    expect(errors).toEqual([]);
}));
test('ManifestMissingFile', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = {
        overrides: [
            { type: 'platform', file: 'aaa\\aaa.windows.js' },
            { type: 'platform', file: 'aaa\\bbb.windows.js' },
        ],
    };
    const expectedError = {
        type: 'fileMissingFromManifest',
        file: 'bbb\\ccc.win32.js',
    };
    const errors = yield new Manifest(manifest, ovrRepo, reactRepo).validate();
    expect(errors).toEqual([expectedError]);
}));
test('ManifestExtraFile', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = {
        overrides: [
            { type: 'platform', file: 'aaa\\aaa.windows.js' },
            { type: 'platform', file: 'aaa\\bbb.windows.js' },
            { type: 'platform', file: 'bbb\\ccc.win32.js' },
            { type: 'platform', file: 'bbb\\ddd.win32.js' },
        ],
    };
    const expectedError = {
        type: 'overrideFileNotFound',
        file: 'bbb\\ddd.win32.js',
    };
    const errors = yield new Manifest(manifest, ovrRepo, reactRepo).validate();
    expect(errors).toEqual([expectedError]);
}));
const testManifestData = {
    overrides: [
        {
            type: 'patch',
            file: overrideFiles[0].filename,
            baseFile: reactFiles[0].filename,
            baseVersion: '0.61.5',
            baseHash: Manifest.hashContent(reactFiles[0].content),
            issue: 4567,
        },
        {
            type: 'derived',
            file: overrideFiles[1].filename,
            baseFile: reactFiles[1].filename,
            baseVersion: '0.60.6',
            baseHash: Manifest.hashContent(reactFiles[1].content),
            issue: 4568,
        },
        {
            type: 'platform',
            file: overrideFiles[2].filename,
        },
    ],
};
test('FullManifestValid', () => __awaiter(void 0, void 0, void 0, function* () {
    const testManifest = new Manifest(testManifestData, ovrRepo, reactRepo);
    const errors = yield testManifest.validate();
    expect(errors).toEqual([]);
}));
test('OutOfDateFile', () => __awaiter(void 0, void 0, void 0, function* () {
    const ourBaseFiles = _.cloneDeep(reactFiles);
    ourBaseFiles[0].content = 'Different than before';
    const ourReactRepo = new MockReactFileRepository(ourBaseFiles);
    const expectedError = {
        type: 'outOfDate',
        file: overrideFiles[0].filename,
    };
    const testManifest = new Manifest(testManifestData, ovrRepo, ourReactRepo);
    const errors = yield testManifest.validate();
    expect(errors).toEqual([expectedError]);
}));
test('BaseFileNotFound', () => __awaiter(void 0, void 0, void 0, function* () {
    const ourManifestData = _.cloneDeep(testManifestData);
    const ovr = ourManifestData.overrides[0];
    ovr.baseFile = 'foo/bar.js';
    const expectedError = {
        type: 'baseFileNotFound',
        file: 'aaa\\aaa.windows.js',
    };
    const testManifest = new Manifest(ourManifestData, ovrRepo, reactRepo);
    const errors = yield testManifest.validate();
    expect(errors).toEqual([expectedError]);
}));
test('HasOverride', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest(testManifestData, ovrRepo, reactRepo);
    expect(manifest.hasOverride(overrideFiles[0].filename)).toBe(true);
}));
test('DoesNotHaveOverride', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest(testManifestData, ovrRepo, reactRepo);
    expect(manifest.hasOverride('Never gonna give you up')).toBe(false);
}));
test('RemoveOverride', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest(testManifestData, ovrRepo, reactRepo);
    expect(manifest.hasOverride(overrideFiles[0].filename)).toBe(true);
    expect(manifest.removeOverride(overrideFiles[0].filename)).toBe(true);
    expect(manifest.hasOverride(overrideFiles[0].filename)).toBe(false);
}));
test('CannotRemoveOverride', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest(testManifestData, ovrRepo, reactRepo);
    expect(manifest.removeOverride('Never gonna let you down')).toBe(false);
}));
test('addOverrideSimple', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    const patch = overrideFiles[0].filename;
    const patchOrig = reactFiles[0].filename;
    yield manifest.addOverride('patch', patch, patchOrig, 1234);
    expect(manifest.hasOverride(patch)).toBe(true);
    const derived = overrideFiles[1].filename;
    const derivedOrig = reactFiles[1].filename;
    yield manifest.addOverride('derived', derived, derivedOrig);
    expect(manifest.hasOverride(derived)).toBe(true);
    const platform = overrideFiles[2].filename;
    yield manifest.addOverride('platform', platform);
    expect(manifest.hasOverride(derived)).toBe(true);
}));
test('addOverrideBadArgs', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    // Missing issue number
    const patch = overrideFiles[0].filename;
    const orig = reactFiles[0].filename;
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addOverride('patch', patch, orig)).rejects.toThrow();
    // Missing issue number and base
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addOverride('patch', patch)).rejects.toThrow();
    // Bad type
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addOverride('potato', patch)).rejects.toThrow();
    // Missing base
    const derived = overrideFiles[1].filename;
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addOverride('derived', derived)).rejects.toThrow();
}));
test('addOverrideTypeSimple', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    const patch = overrideFiles[0].filename;
    const patchOrig = reactFiles[0].filename;
    yield manifest.addPatchOverride(patch, patchOrig, 1234);
    expect(manifest.hasOverride(patch)).toBe(true);
    const derived = overrideFiles[1].filename;
    const derivedOrig = reactFiles[1].filename;
    yield manifest.addDerivedOverride(derived, derivedOrig);
    expect(manifest.hasOverride(derived)).toBe(true);
    const platform = overrideFiles[2].filename;
    yield manifest.addPlatformOverride(platform);
    expect(manifest.hasOverride(derived)).toBe(true);
}));
test('addOverrideDuplicate', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest(testManifestData, ovrRepo, reactRepo);
    const ovr = overrideFiles[0].filename;
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addOverride('platform', ovr)).rejects.toThrow();
}));
test('addOverrideDuplicateNonNormalized', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest(testManifestData, ovrRepo, reactRepo);
    const ovr = overrideFiles[0].filename.replace('\\', '/');
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addOverride('platform', ovr)).rejects.toThrow();
}));
test('addOverrideNoBase', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    const patch = overrideFiles[0].filename;
    const patchOrig = 'Never gonna make you cry';
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addPatchOverride(patch, patchOrig, 1234)).rejects.toThrow();
    const derived = overrideFiles[1].filename;
    const derivedOrig = 'Never gonna say goodbye';
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addDerivedOverride(derived, derivedOrig)).rejects.toThrow();
}));
test('addOverrideNoOverride', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    const patch = 'Never gonna tell a lie and hurt you';
    const patchOrig = reactFiles[0].filename;
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addPatchOverride(patch, patchOrig, 1234)).rejects.toThrow();
    const derived = 'Weve known each other for so long';
    const derivedOrig = reactFiles[1].filename;
    // @ts-ignore Typings don't know about rejects
    expect(manifest.addDerivedOverride(derived, derivedOrig)).rejects.toThrow();
}));
test('addPatchExportedAsData', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    const override = overrideFiles[0].filename;
    const base = reactFiles[0].filename;
    yield manifest.addPatchOverride(override, base, 1234);
    const entryData = manifest.findOverride(override);
    const patchEntryData = entryData;
    expect(patchEntryData.type).toBe('patch');
    expect(patchEntryData.file).toBe(override);
    expect(patchEntryData.baseFile).toBe(base);
    expect(patchEntryData.issue).toBe(1234);
    expect(patchEntryData.baseVersion.length).not.toBe(0);
    expect(patchEntryData.baseHash.length).not.toBe(0);
}));
test('addDerivedExportedAsData', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    const override = overrideFiles[0].filename;
    const base = reactFiles[0].filename;
    yield manifest.addDerivedOverride(override, base, 1234);
    const entryData = manifest.findOverride(override);
    const derivedEntryData = entryData;
    expect(derivedEntryData.type).toBe('derived');
    expect(derivedEntryData.file).toBe(override);
    expect(derivedEntryData.baseFile).toBe(base);
    expect(derivedEntryData.issue).toBe(1234);
    expect(derivedEntryData.baseVersion.length).not.toBe(0);
    expect(derivedEntryData.baseHash.length).not.toBe(0);
}));
test('addDerivedNoIssueExportedAsData', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    const override = overrideFiles[0].filename;
    const base = reactFiles[0].filename;
    yield manifest.addDerivedOverride(override, base);
    const entryData = manifest.findOverride(override);
    const derivedEntryData = entryData;
    expect(derivedEntryData.type).toBe('derived');
    expect(derivedEntryData.file).toBe(override);
    expect(derivedEntryData.baseFile).toBe(base);
    expect(derivedEntryData.issue).toBe(undefined);
    expect(derivedEntryData.baseVersion.length).not.toBe(0);
    expect(derivedEntryData.baseHash.length).not.toBe(0);
}));
test('addPlatformExportedAsData', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    const override = overrideFiles[0].filename;
    yield manifest.addPlatformOverride(override);
    const entryData = manifest.findOverride(override);
    const platformEntryData = entryData;
    expect(platformEntryData.type).toBe('platform');
    expect(platformEntryData.file).toBe(override);
}));
test('ManifestMutateDoesntAffectData', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifestData = { overrides: [] };
    const manifest = new Manifest(manifestData, ovrRepo, reactRepo);
    const patch = overrideFiles[0].filename;
    const patchOrig = reactFiles[0].filename;
    yield manifest.addPatchOverride(patch, patchOrig, 1234);
    expect(manifestData.overrides.length).toBe(0);
}));
test('DataMutateDoesntAffectManifest', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    manifest.getAsData().overrides.push({
        type: 'platform',
        file: 'Your hearts been aching but youre too shy to say it',
    });
    expect(manifest.getAsData().overrides.length).toBe(0);
}));
test('FindOverrideNoneExists', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    expect(manifest.findOverride('foo')).toBe(null);
}));
test('FindOverrideDoesExist', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({
        overrides: [
            {
                type: 'platform',
                file: 'Foo.js',
            },
        ],
    }, ovrRepo, reactRepo);
    // @ts-ignore no typings for toStrictEqual
    expect(manifest.findOverride('Foo.js')).toStrictEqual({
        type: 'platform',
        file: 'Foo.js',
    });
}));
test('MarkUpToDatePlatform', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({
        overrides: [
            {
                type: 'platform',
                file: 'Foo.js',
            },
        ],
    }, ovrRepo, reactRepo);
    // @ts-ignore Typings don't know about rejects
    expect(manifest.markUpToDate('Foo.js')).rejects.toThrow();
}));
test('MarkUpToDateNotFound', () => __awaiter(void 0, void 0, void 0, function* () {
    const manifest = new Manifest({ overrides: [] }, ovrRepo, reactRepo);
    // @ts-ignore Typings don't know about rejects
    expect(manifest.markUpToDate('Foo.js')).rejects.toThrow();
}));
test('MarkUpToDateOutdated', () => __awaiter(void 0, void 0, void 0, function* () {
    const ourTestData = _.cloneDeep(testManifestData);
    const ovr = ourTestData.overrides[0];
    ovr.baseHash = '1234';
    ovr.baseVersion = '0.0.1';
    const manifest = new Manifest(ourTestData, ovrRepo, reactRepo);
    yield manifest.markUpToDate(ovr.file);
    const updated = manifest.findOverride(ovr.file);
    expect(updated.baseVersion).toBe(reactRepo.getVersion());
    expect(updated.baseHash).toBe(Manifest.hashContent(reactFiles[0].content));
}));
//# sourceMappingURL=Manifest.test.js.map