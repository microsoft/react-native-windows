/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Serialized from '../Serialized';
import * as _ from 'lodash';

import {
  CopyOverride,
  DerivedOverride,
  PatchOverride,
  PlatformOverride,
} from '../Override';

import {
  MockFile,
  MockReactFileRepository,
  MockWritableFileRepository,
} from './MockFileRepository';

import Manifest from '../Manifest';
import {OverrideFactoryImpl} from '../OverrideFactory';
import {hashContent} from '../Hash';
import {normalizePath} from '../PathUtils';

const reactFiles: Array<MockFile> = [
  {
    filename: 'aaa\\aaa.js',
    content:
      'I want your love, and I want your revenge;You and me could write a bad romance',
  },
  {
    filename: 'aaa\\bbb.android.js',
    content:
      "Gimme, gimme, gimme a man after midnight. Won't somebody help me chase the shadows away",
  },
  {
    filename: 'bbb\\ccc.ios.js',
    content:
      "Cause honey I'll come get my things, but I can't let go I'm waiting for it, that green light, I want it",
  },
];

const overrideFiles: Array<MockFile> = [
  {
    filename: 'aaa\\aaa.windows.js',
    content:
      'I want your love, and I want your mashed potatoes;You and me could make more mashed potatoes I guess',
  },
  {
    filename: 'aaa\\bbb.windows.js',
    content:
      "Gimme, gimme, gimme 500 live bees after midnight. Won't somebody help me chase the bees out of my house, this is actually quite frightening",
  },
  {
    filename: 'bbb\\ccc.win32.js',
    content:
      "Cause honey I'll come get my things, but I can't let go I'm waiting for it, the fall of civilization as we know it",
  },
  {
    filename: 'overrides.json',
    content: 'Manifest validation should ignore my existence',
  },
  {
    filename: 'node_modules\\foo\\index.js',
    content: 'Manifest validation should ignore my existence as well',
  },
];

const reactRepo = new MockReactFileRepository(reactFiles);
const ovrRepo = new MockWritableFileRepository(overrideFiles);

test('constructor - Duplicate Override Names', () => {
  expect(
    () =>
      new Manifest([
        new PlatformOverride({file: 'aaa.windows.js'}),
        new PlatformOverride({file: 'aaa.windows.js'}),
      ]),
  ).toThrow();
});

test('constructor - Input Mutation Doesnt Affect Manifest', () => {
  const overrides = [
    new PlatformOverride({file: 'aaa.windows.js'}),
    new PlatformOverride({file: 'bbb.windows.js'}),
  ];

  const manifest = new Manifest(overrides);
  expect(manifest.hasOverride('aaa.windows.js')).toBe(true);
  overrides.length = 0;
  expect(manifest.hasOverride('aaa.windows.js')).toBe(true);
});

test('validate - Empty Manifest', async () => {
  const manifest = new Manifest([]);

  const errors = await manifest.validate(
    new MockWritableFileRepository([]),
    reactRepo,
  );
  expect(errors).toEqual([]);
});

test('validate - All Overrides Listed', async () => {
  const manifest = new Manifest([
    new PlatformOverride({file: 'aaa\\aaa.windows.js'}),
    new PlatformOverride({file: 'aaa\\bbb.windows.js'}),
    new PlatformOverride({file: 'bbb\\ccc.win32.js'}),
  ]);

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

test('validate - Missing Override', async () => {
  const manifest = new Manifest([
    new PlatformOverride({file: 'aaa\\aaa.windows.js'}),
    new PlatformOverride({file: 'aaa\\bbb.windows.js'}),
  ]);

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([
    {
      type: 'missingFromManifest',
      overrideName: normalizePath('bbb\\ccc.win32.js'),
    },
  ]);
});

test('validate - Extra Override', async () => {
  const manifest = new Manifest([
    new PlatformOverride({file: 'aaa\\aaa.windows.js'}),
    new PlatformOverride({file: 'aaa\\bbb.windows.js'}),
    new PlatformOverride({file: 'bbb\\ccc.win32.js'}),
    new PlatformOverride({file: 'bbb\\ddd.win32.js'}),
  ]);

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([
    {
      type: 'overrideNotFound',
      overrideName: normalizePath('bbb\\ddd.win32.js'),
    },
  ]);
});

test('validate - Multiple Valid Types', async () => {
  const manifest = new Manifest([
    new PatchOverride({
      file: overrideFiles[0].filename,
      baseFile: reactFiles[0].filename,
      baseVersion: '0.61.5',
      baseHash: hashContent(reactFiles[0].content),
      issue: 4567,
    }),
    new DerivedOverride({
      file: overrideFiles[1].filename,
      baseFile: reactFiles[1].filename,
      baseVersion: '0.60.6',
      baseHash: hashContent(reactFiles[1].content),
      issue: 4568,
    }),
    new PlatformOverride({file: overrideFiles[2].filename}),
  ]);

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

test('validate - Single Include Pattern', async () => {
  const manifest = new Manifest(
    [
      new PlatformOverride({file: 'aaa\\aaa.windows.js'}),
      new PlatformOverride({file: 'aaa\\bbb.windows.js'}),
    ],
    {
      includePatterns: ['**/*.windows.js'],
    },
  );

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

test('validate - Multiple Include Pattern', async () => {
  const manifest = new Manifest(
    [
      new PlatformOverride({file: 'aaa\\aaa.windows.js'}),
      new PlatformOverride({file: 'bbb\\ccc.win32.js'}),
    ],
    {
      includePatterns: ['aaa/aaa*', 'bbb/ccc*'],
    },
  );

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

test('validate - Include Catches Errors', async () => {
  const manifest = new Manifest(
    [new PlatformOverride({file: 'bbb\\ccc.win32.js'})],
    {
      includePatterns: ['aaa/aaa*', 'bbb/ccc*'],
    },
  );

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([
    {
      overrideName: normalizePath('aaa\\aaa.windows.js'),
      type: 'missingFromManifest',
    },
  ]);
});

test('validate - Single Exclude Pattern', async () => {
  const manifest = new Manifest(
    [
      new PlatformOverride({file: 'aaa\\aaa.windows.js'}),
      new PlatformOverride({file: 'aaa\\bbb.windows.js'}),
    ],
    {
      excludePatterns: ['**/*.win32.js'],
    },
  );

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

test('validate - Multiple Exclude Patterns', async () => {
  const manifest = new Manifest(
    [new PlatformOverride({file: 'aaa\\aaa.windows.js'})],
    {
      excludePatterns: ['**/*.win32.js', '**/bbb*'],
    },
  );

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

test('validate - Exclude Catches Errors', async () => {
  const manifest = new Manifest(
    [new PlatformOverride({file: 'aaa\\aaa.windows.js'})],
    {
      excludePatterns: ['**/*.win32.js'],
    },
  );

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([
    {
      overrideName: normalizePath('aaa\\bbb.windows.js'),
      type: 'missingFromManifest',
    },
  ]);
});

test('validate - Include and Exclude Patterns', async () => {
  const manifest = new Manifest(
    [new PlatformOverride({file: 'aaa\\aaa.windows.js'})],
    {
      includePatterns: ['**/*.windows.js'],
      excludePatterns: ['**/bbb*'],
    },
  );

  const errors = await manifest.validate(ovrRepo, reactRepo);
  expect(errors).toEqual([]);
});

const sampleManifest = new Manifest([
  new CopyOverride({
    file: overrideFiles[0].filename,
    baseFile: reactFiles[0].filename,
    baseVersion: '0.62.2',
    baseHash: hashContent(reactFiles[0].content),
    issue: 1234,
  }),
  new DerivedOverride({
    file: overrideFiles[1].filename,
    baseFile: reactFiles[1].filename,
    baseVersion: '0.60.6',
    baseHash: hashContent(reactFiles[1].content),
    issue: 4568,
  }),
  new PlatformOverride({file: overrideFiles[2].filename}),
]);

test('hasOverride - True', () => {
  expect(sampleManifest.hasOverride(overrideFiles[0].filename)).toBe(true);
});

test('hasOverride - False', () => {
  expect(sampleManifest.hasOverride('Never gonna give you up')).toBe(false);
});

test('removeOverride - Success', () => {
  const manifest = _.cloneDeep(sampleManifest);

  expect(manifest.removeOverride(overrideFiles[0].filename)).toBe(true);
  expect(manifest.hasOverride(overrideFiles[0].filename)).toBe(false);
});

test('removeOverride - Does not exist', () => {
  const manifest = _.cloneDeep(sampleManifest);
  expect(manifest.removeOverride('Never gonna let you down')).toBe(false);
});

test('addOverride - Simple', () => {
  const manifest = new Manifest([]);
  manifest.addOverride(
    new PlatformOverride({file: 'Never gonna run around and desert you'}),
  );
  expect(manifest.hasOverride('Never gonna run around and desert you')).toBe(
    true,
  );
});

test('addOverride - Duplicate', () => {
  const manifest = new Manifest([]);
  manifest.addOverride(
    new PlatformOverride({file: 'Never gonna make you cry'}),
  );

  expect(() =>
    manifest.addOverride(
      new PlatformOverride({file: 'Never gonna make you cry'}),
    ),
  ).toThrow();
});

test('findOverride - None exists', () => {
  expect(sampleManifest.findOverride('Never gonna say goodbye')).toBeNull();
});

test('findOverride - Exists', () => {
  const override = new PlatformOverride({
    file: 'Never gonna tell a lie and hurt you',
  });
  const manifest = new Manifest([override]);

  expect(manifest.findOverride('Never gonna tell a lie and hurt you')).toEqual(
    override,
  );
});

test('markUpToDate - Simple', async () => {
  const manifest = new Manifest([
    new CopyOverride({
      file: overrideFiles[0].filename,
      baseFile: reactFiles[0].filename,
      baseVersion: '0.59.2',
      baseHash: 'abcd',
      issue: 1234,
    }),
  ]);

  const overrideFactory = new OverrideFactoryImpl(reactRepo, ovrRepo);
  await manifest.markUpToDate(overrideFiles[0].filename, overrideFactory);

  const newOverride = manifest
    .findOverride(overrideFiles[0].filename)!
    .serialize() as Serialized.CopyOverride;

  expect(newOverride.baseVersion).toBe(reactRepo.getVersion());
  expect(newOverride.baseHash).toBe(hashContent(reactFiles[0].content));
});

test('Serialization (Indivudal Base) Round-Trip', () => {
  const serializedManifest: Serialized.Manifest = {
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: undefined,
    overrides: [
      {
        type: 'platform',
        file: 'aaa.js',
      },
      {
        type: 'derived',
        file: 'abcd.windows.js',
        baseFile: 'abcd.js',
        baseVersion: '0.68.7',
        baseHash: 'gksdpofgkesgpofk',
        issue: undefined,
      },
      {
        type: 'patch',
        file: 'defg.windows.js',
        baseFile: 'defg.js',
        baseVersion: '0.65.3',
        baseHash: 'sdfssfsfsf',
        issue: undefined,
      },
      {
        type: 'copy',
        file: 'ffgg.windows.js',
        baseFile: 'ffgg.android.js',
        baseVersion: '0.65.3',
        baseHash: 'sdfssfsfsf',
        issue: 1234,
      },
    ],
  };

  expect(Manifest.fromSerialized(serializedManifest).serialize()).toEqual(
    serializedManifest,
  );
});

test('Serialization (Default Base) Round-Trip', () => {
  const serializedManifest: Serialized.Manifest = {
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: '0.68.7',
    overrides: [
      {
        type: 'platform',
        file: 'aaa.js',
      },
      {
        type: 'derived',
        file: 'abcd.windows.js',
        baseFile: 'abcd.js',
        baseVersion: undefined,
        baseHash: 'gksdpofgkesgpofk',
        issue: undefined,
      },
      {
        type: 'patch',
        file: 'defg.windows.js',
        baseFile: 'defg.js',
        baseVersion: undefined,
        baseHash: 'sdfssfsfsf',
        issue: undefined,
      },
      {
        type: 'copy',
        file: 'ffgg.windows.js',
        baseFile: 'ffgg.android.js',
        baseVersion: undefined,
        baseHash: 'sdfssfsfsf',
        issue: 1234,
      },
    ],
  };

  expect(Manifest.fromSerialized(serializedManifest).serialize()).toEqual(
    serializedManifest,
  );
});

test('Serialization (Differing Bases) Round-Trip', () => {
  const serializedManifest: Serialized.Manifest = {
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: '0.68.7',
    overrides: [
      {
        type: 'platform',
        file: 'aaa.js',
      },
      {
        type: 'derived',
        file: 'abcd.windows.js',
        baseFile: 'abcd.js',
        baseVersion: undefined,
        baseHash: 'gksdpofgkesgpofk',
        issue: undefined,
      },
      {
        type: 'patch',
        file: 'defg.windows.js',
        baseFile: 'defg.js',
        baseVersion: '0.64.3',
        baseHash: 'sdfssfsfsf',
        issue: undefined,
      },
      {
        type: 'copy',
        file: 'ffgg.windows.js',
        baseFile: 'ffgg.android.js',
        baseVersion: undefined,
        baseHash: 'sdfssfsfsf',
        issue: 1234,
      },
    ],
  };

  expect(Manifest.fromSerialized(serializedManifest).serialize()).toEqual(
    serializedManifest,
  );
});

test('String Exact Serialization Round-Trip', () => {
  const serializedManifest: any = {
    baseVersion: '0.68.7',
    overrides: [
      {
        type: 'platform',
        file: 'aaa.js',
      },
      {
        type: 'derived',
        file: 'abcd.windows.js',
        baseFile: 'abcd.js',
        baseHash: 'gksdpofgkesgpofk',
      },
      {
        type: 'patch',
        file: 'defg.windows.js',
        baseFile: 'defg.js',
        baseVersion: '0.65.3',
        baseHash: 'sdfssfsfsf',
        issue: undefined,
      },
      {
        type: 'copy',
        file: 'ffgg.windows.js',
        baseFile: 'ffgg.android.js',
        baseHash: 'sdfssfsfsf',
        issue: 1234,
      },
    ],
  };

  const manifestString = JSON.stringify(serializedManifest);
  const manifest = Manifest.fromSerialized(
    Serialized.parseManifest(manifestString),
  );
  expect(JSON.stringify(manifest.serialize())).toEqual(manifestString);
});

test('String Exact Serialization Round-Trip (With Patterns)', () => {
  const serializedManifest: any = {
    includePatterns: ['src/**'],
    excludePatterns: ['**/*.png'],
    overrides: [
      {
        type: 'copy',
        file: 'ffgg.windows.js',
        baseFile: 'ffgg.android.js',
        baseVersion: '0.65.3',
        baseHash: 'sdfssfsfsf',
        issue: 1234,
      },
    ],
  };

  const manifestString = JSON.stringify(serializedManifest);
  const manifest = Manifest.fromSerialized(
    Serialized.parseManifest(manifestString),
  );
  expect(JSON.stringify(manifest.serialize())).toEqual(manifestString);
});
