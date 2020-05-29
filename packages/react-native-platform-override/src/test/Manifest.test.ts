// /**
//  * Copyright (c) Microsoft Corporation.
//  * Licensed under the MIT License.
//  *
//  * @format
//  */

// import * as Serialized from '../Serialized';
// import * as _ from 'lodash';
// import Manifest, {ValidationError} from '../Manifest';

// import {
//   MockFile,
//   MockOverrideFileRepository,
//   MockReactFileRepository,
// } from './MockFileRepository';

// const reactFiles: Array<MockFile> = [
//   {
//     filename: 'aaa\\aaa.js',
//     content:
//       'I want your love, and I want your revenge;You and me could write a bad romance',
//   },
//   {
//     filename: 'aaa\\bbb.android.js',
//     content:
//       "Gimme, gimme, gimme a man after midnight. Won't somebody help me chase the shadows away",
//   },
//   {
//     filename: 'bbb\\ccc.ios.js',
//     content:
//       "Cause honey I'll come get my things, but I can't let go I'm waiting for it, that green light, I want it",
//   },
// ];

// const overrideFiles: Array<MockFile> = [
//   {
//     filename: 'aaa\\aaa.windows.js',
//     content:
//       'I want your love, and I want your mashed potatoes;You and me could make more mashed potatoes I guess',
//   },
//   {
//     filename: 'aaa\\bbb.windows.js',
//     content:
//       "Gimme, gimme, gimme 500 live bees after midnight. Won't somebody help me chase the bees out of my house, this is actually quite frightening",
//   },
//   {
//     filename: 'bbb\\ccc.win32.js',
//     content:
//       "Cause honey I'll come get my things, but I can't let go I'm waiting for it, the fall of civilization as we know it",
//   },
// ];

// const reactRepo = new MockReactFileRepository(reactFiles);
// const ovrRepo = new MockOverrideFileRepository(overrideFiles);

// test('AllListedInManifest', async () => {
//   const manifest: Serialized.Manifest = {
//     overrides: [
//       {type: 'platform', file: 'aaa\\aaa.windows.js'},
//       {type: 'platform', file: 'aaa\\bbb.windows.js'},
//       {type: 'platform', file: 'bbb\\ccc.win32.js'},
//     ],
//   };

//   const errors = await new Manifest(manifest, ovrRepo, reactRepo).validate();
//   expect(errors).toEqual([]);
// });

// test('ManifestMissingFile', async () => {
//   const manifest: Serialized.Manifest = {
//     overrides: [
//       {type: 'platform', file: 'aaa\\aaa.windows.js'},
//       {type: 'platform', file: 'aaa\\bbb.windows.js'},
//     ],
//   };

//   const expectedError: ValidationError = {
//     type: 'fileMissingFromManifest',
//     file: 'bbb\\ccc.win32.js',
//   };

//   const errors = await new Manifest(manifest, ovrRepo, reactRepo).validate();
//   expect(errors).toEqual([expectedError]);
// });

// test('ManifestExtraFile', async () => {
//   const manifest: Serialized.Manifest = {
//     overrides: [
//       {type: 'platform', file: 'aaa\\aaa.windows.js'},
//       {type: 'platform', file: 'aaa\\bbb.windows.js'},
//       {type: 'platform', file: 'bbb\\ccc.win32.js'},
//       {type: 'platform', file: 'bbb\\ddd.win32.js'},
//     ],
//   };

//   const expectedError: ValidationError = {
//     type: 'overrideFileNotFound',
//     file: 'bbb\\ddd.win32.js',
//   };

//   const errors = await new Manifest(manifest, ovrRepo, reactRepo).validate();
//   expect(errors).toEqual([expectedError]);
// });

// const testSerialized: Serialized.Manifest = {
//   overrides: [
//     {
//       type: 'patch',
//       file: overrideFiles[0].filename,
//       baseFile: reactFiles[0].filename,
//       baseVersion: '0.61.5',
//       baseHash: Manifest.hashContent(reactFiles[0].content),
//       issue: 4567,
//     },
//     {
//       type: 'derived',
//       file: overrideFiles[1].filename,
//       baseFile: reactFiles[1].filename,
//       baseVersion: '0.60.6',
//       baseHash: Manifest.hashContent(reactFiles[1].content),
//       issue: 4568,
//     },
//     {
//       type: 'platform',
//       file: overrideFiles[2].filename,
//     },
//   ],
// };

// test('FullManifestValid', async () => {
//   const testManifest = new Manifest(testSerialized, ovrRepo, reactRepo);
//   const errors = await testManifest.validate();
//   expect(errors).toEqual([]);
// });

// test('OutOfDateFile', async () => {
//   const ourBaseFiles = _.cloneDeep(reactFiles);
//   ourBaseFiles[0].content = 'Different than before';
//   const ourReactRepo = new MockReactFileRepository(ourBaseFiles);

//   const expectedError: ValidationError = {
//     type: 'outOfDate',
//     file: overrideFiles[0].filename,
//   };

//   const testManifest = new Manifest(testSerialized, ovrRepo, ourReactRepo);
//   const errors = await testManifest.validate();
//   expect(errors).toEqual([expectedError]);
// });

// test('BaseFileNotFound', async () => {
//   const ourSerialized = _.cloneDeep(testSerialized);
//   const ovr = ourSerialized.overrides[0] as Serialized.PatchOverride;
//   ovr.baseFile = 'foo/bar.js';

//   const expectedError: ValidationError = {
//     type: 'baseFileNotFound',
//     file: 'aaa\\aaa.windows.js',
//   };

//   const testManifest = new Manifest(ourSerialized, ovrRepo, reactRepo);
//   const errors = await testManifest.validate();
//   expect(errors).toEqual([expectedError]);
// });

// test('HasOverride', async () => {
//   const manifest = new Manifest(testSerialized, ovrRepo, reactRepo);
//   expect(manifest.hasOverride(overrideFiles[0].filename)).toBe(true);
// });

// test('DoesNotHaveOverride', async () => {
//   const manifest = new Manifest(testSerialized, ovrRepo, reactRepo);
//   expect(manifest.hasOverride('Never gonna give you up')).toBe(false);
// });

// test('RemoveOverride', async () => {
//   const manifest = new Manifest(testSerialized, ovrRepo, reactRepo);

//   expect(manifest.hasOverride(overrideFiles[0].filename)).toBe(true);
//   expect(manifest.removeOverride(overrideFiles[0].filename)).toBe(true);
//   expect(manifest.hasOverride(overrideFiles[0].filename)).toBe(false);
// });

// test('CannotRemoveOverride', async () => {
//   const manifest = new Manifest(testSerialized, ovrRepo, reactRepo);
//   expect(manifest.removeOverride('Never gonna let you down')).toBe(false);
// });

// test('addOverrideSimple', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   const patch = overrideFiles[0].filename;
//   const patchOrig = reactFiles[0].filename;
//   await manifest.addOverride('patch', patch, patchOrig, 1234);
//   expect(manifest.hasOverride(patch)).toBe(true);

//   const derived = overrideFiles[1].filename;
//   const derivedOrig = reactFiles[1].filename;
//   await manifest.addOverride('derived', derived, derivedOrig);
//   expect(manifest.hasOverride(derived)).toBe(true);

//   const platform = overrideFiles[2].filename;
//   await manifest.addOverride('platform', platform);
//   expect(manifest.hasOverride(derived)).toBe(true);
// });

// test('addOverrideBadArgs', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   // Missing issue number
//   const patch = overrideFiles[0].filename;
//   const orig = reactFiles[0].filename;
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addOverride('patch', patch, orig)).rejects.toThrow();

//   // Missing issue number and base
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addOverride('patch', patch)).rejects.toThrow();

//   // Bad type
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addOverride('potato', patch)).rejects.toThrow();

//   // Missing base
//   const derived = overrideFiles[1].filename;
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addOverride('derived', derived)).rejects.toThrow();
// });

// test('addOverrideTypeSimple', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   const patch = overrideFiles[0].filename;
//   const patchOrig = reactFiles[0].filename;
//   await manifest.addPatchOverride(patch, patchOrig, 1234);
//   expect(manifest.hasOverride(patch)).toBe(true);

//   const derived = overrideFiles[1].filename;
//   const derivedOrig = reactFiles[1].filename;
//   await manifest.addDerivedOverride(derived, derivedOrig);
//   expect(manifest.hasOverride(derived)).toBe(true);

//   const platform = overrideFiles[2].filename;
//   await manifest.addPlatformOverride(platform);
//   expect(manifest.hasOverride(derived)).toBe(true);
// });

// test('addOverrideDuplicate', async () => {
//   const manifest = new Manifest(testSerialized, ovrRepo, reactRepo);

//   const ovr = overrideFiles[0].filename;
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addOverride('platform', ovr)).rejects.toThrow();
// });

// test('addOverrideDuplicateNonNormalized', async () => {
//   const manifest = new Manifest(testSerialized, ovrRepo, reactRepo);

//   const ovr = overrideFiles[0].filename.replace('\\', '/');
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addOverride('platform', ovr)).rejects.toThrow();
// });

// test('addOverrideNoBase', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   const patch = overrideFiles[0].filename;
//   const patchOrig = 'Never gonna make you cry';
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addPatchOverride(patch, patchOrig, 1234)).rejects.toThrow();

//   const derived = overrideFiles[1].filename;
//   const derivedOrig = 'Never gonna say goodbye';
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addDerivedOverride(derived, derivedOrig)).rejects.toThrow();
// });

// test('addOverrideNoOverride', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   const patch = 'Never gonna tell a lie and hurt you';
//   const patchOrig = reactFiles[0].filename;
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addPatchOverride(patch, patchOrig, 1234)).rejects.toThrow();

//   const derived = 'Weve known each other for so long';
//   const derivedOrig = reactFiles[1].filename;
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.addDerivedOverride(derived, derivedOrig)).rejects.toThrow();
// });

// test('addPatchExportedAsData', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   const override = overrideFiles[0].filename;
//   const base = reactFiles[0].filename;
//   await manifest.addPatchOverride(override, base, 1234);

//   const entryData = manifest.findOverride(override);

//   const patchEntryData = entryData as Serialized.DerivedOverride;
//   expect(patchEntryData.type).toBe('patch');
//   expect(patchEntryData.file).toBe(override);
//   expect(patchEntryData.baseFile).toBe(base);
//   expect(patchEntryData.issue).toBe(1234);
//   expect(patchEntryData.baseVersion.length).not.toBe(0);
//   expect(patchEntryData.baseHash.length).not.toBe(0);
// });

// test('addDerivedExportedAsData', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   const override = overrideFiles[0].filename;
//   const base = reactFiles[0].filename;
//   await manifest.addDerivedOverride(override, base, 1234);

//   const entryData = manifest.findOverride(override);

//   const derivedEntryData = entryData as Serialized.DerivedOverride;
//   expect(derivedEntryData.type).toBe('derived');
//   expect(derivedEntryData.file).toBe(override);
//   expect(derivedEntryData.baseFile).toBe(base);
//   expect(derivedEntryData.issue).toBe(1234);
//   expect(derivedEntryData.baseVersion.length).not.toBe(0);
//   expect(derivedEntryData.baseHash.length).not.toBe(0);
// });

// test('addDerivedNoIssueExportedAsData', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   const override = overrideFiles[0].filename;
//   const base = reactFiles[0].filename;
//   await manifest.addDerivedOverride(override, base);

//   const entryData = manifest.findOverride(override);

//   const derivedEntryData = entryData as Serialized.DerivedOverride;
//   expect(derivedEntryData.type).toBe('derived');
//   expect(derivedEntryData.file).toBe(override);
//   expect(derivedEntryData.baseFile).toBe(base);
//   expect(derivedEntryData.issue).toBe(undefined);
//   expect(derivedEntryData.baseVersion.length).not.toBe(0);
//   expect(derivedEntryData.baseHash.length).not.toBe(0);
// });

// test('addPlatformExportedAsData', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);

//   const override = overrideFiles[0].filename;
//   await manifest.addPlatformOverride(override);

//   const entryData = manifest.findOverride(override);

//   const platformEntryData = entryData as Serialized.PlatformOverride;
//   expect(platformEntryData.type).toBe('platform');
//   expect(platformEntryData.file).toBe(override);
// });

// test('ManifestMutateDoesntAffectData', async () => {
//   const Serialized: Serialized.Manifest = {overrides: []};
//   const manifest = new Manifest(Serialized, ovrRepo, reactRepo);

//   const patch = overrideFiles[0].filename;
//   const patchOrig = reactFiles[0].filename;
//   await manifest.addPatchOverride(patch, patchOrig, 1234);

//   expect(Serialized.overrides.length).toBe(0);
// });

// test('DataMutateDoesntAffectManifest', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);
//   manifest.getAsData().overrides.push({
//     type: 'platform',
//     file: 'Your hearts been aching but youre too shy to say it',
//   });

//   expect(manifest.getAsData().overrides.length).toBe(0);
// });

// test('FindOverrideNoneExists', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);
//   expect(manifest.findOverride('foo')).toBe(null);
// });

// test('FindOverrideDoesExist', async () => {
//   const manifest = new Manifest(
//     {
//       overrides: [
//         {
//           type: 'platform',
//           file: 'Foo.js',
//         },
//       ],
//     },
//     ovrRepo,
//     reactRepo,
//   );
//   // @ts-ignore no typings for toStrictEqual
//   expect(manifest.findOverride('Foo.js')).toStrictEqual({
//     type: 'platform',
//     file: 'Foo.js',
//   });
// });

// test('MarkUpToDatePlatform', async () => {
//   const manifest = new Manifest(
//     {
//       overrides: [
//         {
//           type: 'platform',
//           file: 'Foo.js',
//         },
//       ],
//     },
//     ovrRepo,
//     reactRepo,
//   );

//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.markUpToDate('Foo.js')).rejects.toThrow();
// });

// test('MarkUpToDateNotFound', async () => {
//   const manifest = new Manifest({overrides: []}, ovrRepo, reactRepo);
//   // @ts-ignore Typings don't know about rejects
//   expect(manifest.markUpToDate('Foo.js')).rejects.toThrow();
// });

// test('MarkUpToDateOutdated', async () => {
//   const ourTestData = _.cloneDeep(testSerialized);
//   const ovr = ourTestData.overrides[0] as Serialized.NonPlatformOverride;
//   ovr.baseHash = '1234';
//   ovr.baseVersion = '0.0.1';

//   const manifest = new Manifest(ourTestData, ovrRepo, reactRepo);
//   await manifest.markUpToDate(ovr.file);

//   const updated = manifest.findOverride(
//     ovr.file,
//   ) as Serialized.NonPlatformOverride;
//   expect(updated.baseVersion).toBe(reactRepo.getVersion());
//   expect(updated.baseHash).toBe(Manifest.hashContent(reactFiles[0].content));
// });
