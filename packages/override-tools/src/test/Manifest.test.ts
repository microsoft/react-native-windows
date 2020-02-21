/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Manifest from '../Manifest';

import {
  MockFile,
  MockOverrideFileRepository,
  MockReactFileRepository,
} from './MockFileRepository';

test('EmptyManifestInvalid', () => {
  expect(() => Manifest.parse('{}')).toThrow();
});

test('WellFormedNoOverrides', () => {
  expect(Manifest.parse('{"overrides": []}')).toEqual({overrides: []});
});

test('WellFormedPlatform', () => {
  const manifest: Manifest.Manifest = {
    overrides: [
      {
        type: 'platform',
        file: 'foo.win32.js',
      },
    ],
  };

  expect(Manifest.parse(JSON.stringify(manifest))).toEqual(manifest);
});

test('WellFormedPatch', () => {
  const manifest: Manifest.Manifest = {
    overrides: [
      {
        type: 'patch',
        file: 'foo.win32.js',
        baseFile: 'foo.js',
        baseVersion: '0.61.5',
        baseHash: 'AAAABBBB',
        issue: 4567,
      },
    ],
  };

  expect(Manifest.parse(JSON.stringify(manifest))).toEqual(manifest);
});

test('WellFormedDerived', () => {
  const manifest: Manifest.Manifest = {
    overrides: [
      {
        type: 'derived',
        file: 'foo.win32.js',
        baseFile: 'foo.js',
        baseVersion: '0.61.5',
        baseHash: 'AAAABBBB',
        issue: 4567,
      },
    ],
  };

  expect(Manifest.parse(JSON.stringify(manifest))).toEqual(manifest);
});

test('FixmeAllowedAsIssue', () => {
  const manifest: Manifest.Manifest = {
    overrides: [
      {
        type: 'patch',
        file: 'foo.win32.js',
        baseFile: 'foo.js',
        baseVersion: '0.61.5',
        baseHash: 'AAAABBBB',
        issue: 'LEGACY_FIXME',
      },
    ],
  };

  expect(Manifest.parse(JSON.stringify(manifest))).toEqual(manifest);
});

test('IssueMustBePresentForPatch', () => {
  const manifest = {
    overrides: [
      {
        type: 'patch',
        file: 'foo.win32.js',
        baseFile: 'foo.js',
        baseVersion: '0.61.5',
        baseHash: 'AAAABBBB',
      },
    ],
  };

  expect(() => Manifest.parse(JSON.stringify(manifest))).toThrow();
});

test('IssueCannotBeArbitraryString', () => {
  const manifest = {
    overrides: [
      {
        type: 'patch',
        file: 'foo.win32.js',
        baseFile: 'foo.js',
        baseVersion: '0.61.5',
        baseHash: 'AAAABBBB',
        issue: 'bad',
      },
    ],
  };

  expect(() => Manifest.parse(JSON.stringify(manifest))).toThrow();
});

test('WellFormedPatchMustHaveMetadata', () => {
  const manifest = {
    overrides: [
      {
        type: 'patch',
        file: 'foo.win32.js',
      },
    ],
  };

  expect(() => Manifest.parse(JSON.stringify(manifest))).toThrow();
});

test('WellFormedDerivedMustHaveMetadata', () => {
  const manifest = {
    overrides: [
      {
        type: 'derived',
        file: 'foo.win32.js',
      },
    ],
  };

  expect(() => Manifest.parse(JSON.stringify(manifest))).toThrow();
});

const reactFiles: Array<MockFile> = [
  {
    filename: 'aaa/aaa.js',
    content:
      'I want your love, and I want your revenge;You and me could write a bad romance',
  },
  {
    filename: 'aaa/bbb.android.js',
    content:
      "Gimme, gimme, gimme a man after midnight. Won't somebody help me chase the shadows away",
  },
  {
    filename: 'bbb/ccc.ios.js',
    content:
      "Cause honey I'll come get my things, but I can't let go I'm waiting for it, that green light, I want it",
  },
];

const overrideFiles: Array<MockFile> = [
  {
    filename: 'aaa/aaa.windows.js',
    content:
      'I want your love, and I want your mashed potatoes;You and me could make more mashed potatoes I guess',
  },
  {
    filename: 'aaa/bbb.windows.js',
    content:
      "Gimme, gimme, gimme 500 live bees after midnight. Won't somebody help me chase the bees out of my house, this is actually quite frightening",
  },
  {
    filename: 'bbb/ccc.win32.js',
    content:
      "Cause honey I'll come get my things, but I can't let go I'm waiting for it, the fall of civilization as we know it",
  },
];

const reactRepo = new MockReactFileRepository(reactFiles);
const ovrRepo = new MockOverrideFileRepository(overrideFiles);

test('AllListedInManifest', async () => {
  const manifest: Manifest.Manifest = {
    overrides: [
      {type: 'platform', file: 'aaa/aaa.windows.js'},
      {type: 'platform', file: 'aaa/bbb.windows.js'},
      {type: 'platform', file: 'bbb/ccc.win32.js'},
    ],
  };

  const errors = await Manifest.validate(manifest, ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

test('ManifestMissingFile', async () => {
  const manifest: Manifest.Manifest = {
    overrides: [
      {type: 'platform', file: 'aaa/aaa.windows.js'},
      {type: 'platform', file: 'aaa/bbb.windows.js'},
    ],
  };

  const expectedError: Manifest.ValidationError = {
    type: 'fileMissingFromManifest',
    file: 'bbb/ccc.win32.js',
  };

  const errors = await Manifest.validate(manifest, ovrRepo, reactRepo);
  expect(errors).toEqual([expectedError]);
});

test('ManifestExtraFile', async () => {
  const manifest: Manifest.Manifest = {
    overrides: [
      {type: 'platform', file: 'aaa/aaa.windows.js'},
      {type: 'platform', file: 'aaa/bbb.windows.js'},
      {type: 'platform', file: 'bbb/ccc.win32.js'},
      {type: 'platform', file: 'bbb/ddd.win32.js'},
    ],
  };

  const expectedError: Manifest.ValidationError = {
    type: 'overrideFileNotFound',
    file: 'bbb/ddd.win32.js',
  };

  const errors = await Manifest.validate(manifest, ovrRepo, reactRepo);
  expect(errors).toEqual([expectedError]);
});

const testManifest: Manifest.Manifest = {
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

test('FullManifestValid', async () => {
  const errors = await Manifest.validate(testManifest, ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

test('OutOfDateFile', async () => {
  const ourBaseFiles = reactFiles;
  ourBaseFiles[0].content = 'Different than before';
  const ourReactRepo = new MockReactFileRepository(ourBaseFiles);

  const expectedError: Manifest.ValidationError = {
    type: 'outOfDate',
    file: overrideFiles[0].filename,
  };

  const errors = await Manifest.validate(testManifest, ovrRepo, ourReactRepo);
  expect(errors).toEqual([expectedError]);
});

test('BaseFileNotFound', async () => {
  const ourManifest = testManifest;
  const ovr = ourManifest.overrides[0] as Manifest.NonPlatformEntry;
  ovr.baseFile = 'foo/bar.js';

  const expectedError: Manifest.ValidationError = {
    type: 'baseFileNotFound',
    file: 'foo/bar.js',
  };

  const errors = await Manifest.validate(ourManifest, ovrRepo, reactRepo);
  expect(errors).toEqual([expectedError]);
});
