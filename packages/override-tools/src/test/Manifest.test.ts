/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as ManifestData from '../ManifestData';
import Manifest, {ValidationError} from '../Manifest';

import {
  MockFile,
  MockOverrideFileRepository,
  MockReactFileRepository,
} from './MockFileRepository';

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
  const manifest: ManifestData.Manifest = {
    overrides: [
      {type: 'platform', file: 'aaa/aaa.windows.js'},
      {type: 'platform', file: 'aaa/bbb.windows.js'},
      {type: 'platform', file: 'bbb/ccc.win32.js'},
    ],
  };

  const errors = await new Manifest(manifest, ovrRepo, reactRepo).validate();
  expect(errors).toEqual([]);
});

test('ManifestMissingFile', async () => {
  const manifest: ManifestData.Manifest = {
    overrides: [
      {type: 'platform', file: 'aaa/aaa.windows.js'},
      {type: 'platform', file: 'aaa/bbb.windows.js'},
    ],
  };

  const expectedError: ValidationError = {
    type: 'fileMissingFromManifest',
    file: 'bbb/ccc.win32.js',
  };

  const errors = await new Manifest(manifest, ovrRepo, reactRepo).validate();
  expect(errors).toEqual([expectedError]);
});

test('ManifestExtraFile', async () => {
  const manifest: ManifestData.Manifest = {
    overrides: [
      {type: 'platform', file: 'aaa/aaa.windows.js'},
      {type: 'platform', file: 'aaa/bbb.windows.js'},
      {type: 'platform', file: 'bbb/ccc.win32.js'},
      {type: 'platform', file: 'bbb/ddd.win32.js'},
    ],
  };

  const expectedError: ValidationError = {
    type: 'overrideFileNotFound',
    file: 'bbb/ddd.win32.js',
  };

  const errors = await new Manifest(manifest, ovrRepo, reactRepo).validate();
  expect(errors).toEqual([expectedError]);
});

const testManifestData: ManifestData.Manifest = {
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
  const testManifest = new Manifest(testManifestData, ovrRepo, reactRepo);
  const errors = await testManifest.validate();
  expect(errors).toEqual([]);
});

test('OutOfDateFile', async () => {
  const ourBaseFiles = reactFiles;
  ourBaseFiles[0].content = 'Different than before';
  const ourReactRepo = new MockReactFileRepository(ourBaseFiles);

  const expectedError: ValidationError = {
    type: 'outOfDate',
    file: overrideFiles[0].filename,
  };

  const testManifest = new Manifest(testManifestData, ovrRepo, ourReactRepo);
  const errors = await testManifest.validate();
  expect(errors).toEqual([expectedError]);
});

test('BaseFileNotFound', async () => {
  const ourManifestData = testManifestData;
  const ovr = ourManifestData.overrides[0] as ManifestData.PatchEntry;
  ovr.baseFile = 'foo/bar.js';

  const expectedError: ValidationError = {
    type: 'baseFileNotFound',
    file: 'foo/bar.js',
  };

  const testManifest = new Manifest(ourManifestData, ovrRepo, reactRepo);
  const errors = await testManifest.validate();
  expect(errors).toEqual([expectedError]);
});
