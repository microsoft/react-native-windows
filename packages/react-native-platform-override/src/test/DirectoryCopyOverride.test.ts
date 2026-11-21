/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import path from 'path';
import inquirer from 'inquirer';

import GitReactFileRepository from '../GitReactFileRepository';
import {hashFileOrDirectory} from '../Hash';
import Manifest from '../Manifest';
import {
  DirectoryCopyOverride,
  PlatformOverride,
  deserializeOverride,
} from '../Override';
import {OverrideFactoryImpl} from '../OverrideFactory';
import {overrideFromDetails, promptForOverrideDetails} from '../OverridePrompt';
import * as FileSearch from '../FileSearch';
import {parseManifest} from '../Serialized';
import {usingScratchDirectory} from '../e2etest/Resource';
import {
  MockReactFileRepository,
  MockWritableFileRepository,
} from './MockFileRepository';

const excludePatterns = ['config.js', 'local/**', '**/.private'];
const upstreamFiles = [
  {filename: 'upstream/keep.js', content: 'shared'},
  {filename: 'upstream/config.js', content: 'upstream config'},
  {filename: 'upstream/local/new.js', content: 'do not copy'},
  {filename: 'upstream/nested/.private', content: 'upstream secret'},
];
const localFiles = [
  {filename: 'mirror/keep.js', content: 'shared'},
  {filename: 'mirror/config.js', content: 'custom config'},
  {filename: 'mirror/local/old.js', content: 'do not delete'},
  {filename: 'mirror/nested/.private', content: 'custom secret'},
];

function context() {
  const reactRepo = new MockReactFileRepository(upstreamFiles);
  const overrideRepo = new MockWritableFileRepository(localFiles);
  const factory = new OverrideFactoryImpl(reactRepo, overrideRepo);
  return {reactRepo, overrideRepo, factory};
}

afterEach(() => {
  jest.restoreAllMocks();
});

test('exclusions round-trip and are relative to the copied directory', async () => {
  const {factory} = context();
  const patterns = [...excludePatterns];
  const override = await factory.createDirectoryCopyOverride(
    'mirror',
    'upstream',
    123,
    patterns,
  );
  patterns.push('keep.js');
  expect(override.includesFile('mirror/keep.js')).toBe(true);
  expect(override.includesFile('other/keep.js')).toBe(false);
  expect(override.includesFile('mirror/config.js')).toBe(false);
  expect(override.includesFile('mirror\\local\\old.js')).toBe(false);
  expect(override.includesFile('mirror/nested/.private')).toBe(false);
  expect(override.serialize().excludePatterns).toEqual(excludePatterns);
  expect(deserializeOverride(override.serialize())).toEqual(override);
  const serialized = override.serialize();
  serialized.excludePatterns?.push('keep.js');
  expect(override.includesFile('mirror/keep.js')).toBe(true);
});

test('accepts empty exclusions and legacy directory-copy manifests', async () => {
  const {factory} = context();
  for (const patterns of [undefined, []]) {
    const override = await factory.createDirectoryCopyOverride(
      'mirror',
      'upstream',
      undefined,
      patterns,
    );
    expect(override.includesFile('mirror/config.js')).toBe(true);
    expect(override.serialize().baseHash).toBe(
      await hashFileOrDirectory('upstream', context().reactRepo),
    );
    expect(deserializeOverride(override.serialize())).toEqual(override);
  }
});

test.each([{patterns: ['config.js']}, {patterns: []}, {patterns: undefined}])(
  'validates serialized exclusion patterns: %p',
  async ({patterns}) => {
    const {factory} = context();
    const override = await factory.createDirectoryCopyOverride(
      'mirror',
      'upstream',
      undefined,
      patterns,
    );
    const serialized = {overrides: [override.serialize()]};
    expect(parseManifest(JSON.stringify(serialized))).toEqual(serialized);
  },
);

test.each([{patterns: 'config.js'}, {patterns: [123]}, {patterns: null}])(
  'rejects malformed exclusion patterns: %p',
  async ({patterns}) => {
    const {factory} = context();
    const override = await factory.createDirectoryCopyOverride(
      'mirror',
      'upstream',
    );
    expect(() =>
      parseManifest(
        JSON.stringify({
          overrides: [{...override.serialize(), excludePatterns: patterns}],
        }),
      ),
    ).toThrow('Could not parse manifest');
  },
);

test('hashes only included upstream files and preserves legacy hashes', async () => {
  const {factory, reactRepo} = context();
  const override = await factory.createDirectoryCopyOverride(
    'mirror',
    'upstream',
    undefined,
    excludePatterns,
  );
  const includedRepo = new MockReactFileRepository([upstreamFiles[0]]);
  expect(override.serialize().baseHash).toBe(
    await hashFileOrDirectory('upstream', includedRepo),
  );
  expect(override.serialize().baseHash).not.toBe(
    await hashFileOrDirectory('upstream', reactRepo),
  );
});

test('excluded files require separate registration and are ignored by copy validation', async () => {
  const {factory, reactRepo, overrideRepo} = context();
  const override = await factory.createDirectoryCopyOverride(
    'mirror',
    'upstream',
    undefined,
    excludePatterns,
  );
  const manifest = new Manifest([override], {includePatterns: ['mirror/**']});
  expect(await manifest.validate(overrideRepo, reactRepo)).toEqual([
    {
      type: 'missingFromManifest',
      overrideName: path.normalize('mirror/config.js'),
    },
    {
      type: 'missingFromManifest',
      overrideName: path.normalize('mirror/local/old.js'),
    },
    {
      type: 'missingFromManifest',
      overrideName: path.normalize('mirror/nested/.private'),
    },
  ]);
  for (const file of localFiles.slice(1)) {
    manifest.addOverride(new PlatformOverride({file: file.filename}));
  }
  expect(await manifest.validate(overrideRepo, reactRepo)).toEqual([]);
  await overrideRepo.writeFile(
    path.normalize('mirror/keep.js'),
    Buffer.from('modified'),
  );
  expect(await manifest.validate(overrideRepo, reactRepo)).toEqual([
    {type: 'overrideDifferentFromBase', overrideName: 'mirror'},
  ]);
});

test('excluded upstream changes do not make a directory copy out of date', async () => {
  const {factory, overrideRepo} = context();
  const override = await factory.createDirectoryCopyOverride(
    'mirror',
    'upstream',
    undefined,
    excludePatterns,
  );
  const manifest = new Manifest([override], {
    includePatterns: ['mirror/keep.js'],
  });
  const changedExcludedRepo = new MockReactFileRepository([
    upstreamFiles[0],
    {filename: 'upstream/config.js', content: 'changed excluded file'},
  ]);
  expect(await manifest.validate(overrideRepo, changedExcludedRepo)).toEqual(
    [],
  );
  const changedIncludedRepo = new MockReactFileRepository([
    {filename: 'upstream/keep.js', content: 'changed included file'},
  ]);
  expect(await manifest.validate(overrideRepo, changedIncludedRepo)).toEqual([
    {type: 'outOfDate', overrideName: 'mirror'},
    {type: 'overrideDifferentFromBase', overrideName: 'mirror'},
  ]);
});

test('upgrading preserves excluded files, skips excluded additions, and refreshes the filtered hash', async () => {
  const {factory, overrideRepo} = context();
  const override = await factory.createDirectoryCopyOverride(
    'mirror',
    'upstream',
    123,
    excludePatterns,
  );
  const newReactRepo = new MockReactFileRepository(
    [...upstreamFiles, {filename: 'upstream/new.js', content: 'added'}].map(
      file =>
        file.filename === 'upstream/keep.js'
          ? {...file, content: 'updated'}
          : file,
    ),
  );
  await overrideRepo.writeFile(
    path.normalize('mirror/deleted.js'),
    Buffer.from('remove me'),
  );
  await usingScratchDirectory(async dir => {
    const gitRepo = await GitReactFileRepository.createAndInit(dir);
    jest
      .spyOn(gitRepo, 'listFiles')
      .mockImplementation(globs => newReactRepo.listFiles(globs));
    jest
      .spyOn(gitRepo, 'readFile')
      .mockImplementation(file => newReactRepo.readFile(file));
    jest
      .spyOn(gitRepo, 'stat')
      .mockImplementation(file => newReactRepo.stat(file));
    expect(
      await override
        .upgradeStrategy()
        .upgrade(gitRepo, overrideRepo, '0.62.2', false),
    ).toEqual({
      overrideName: 'mirror',
      filesWritten: true,
      hasConflicts: false,
    });
  });
  expect((await overrideRepo.readFile('mirror/keep.js'))?.toString()).toBe(
    'updated',
  );
  expect((await overrideRepo.readFile('mirror/new.js'))?.toString()).toBe(
    'added',
  );
  expect(await overrideRepo.readFile('mirror/deleted.js')).toBeNull();
  expect(await overrideRepo.readFile('mirror/local/new.js')).toBeNull();
  for (const file of localFiles.slice(1)) {
    expect((await overrideRepo.readFile(file.filename))?.toString()).toBe(
      file.content,
    );
  }
  const updated = await override.createUpdated(
    new OverrideFactoryImpl(newReactRepo, overrideRepo),
  );
  expect(updated.serialize()).toMatchObject({
    excludePatterns,
    issue: 123,
    baseHash: await hashFileOrDirectory('upstream', newReactRepo, {
      excludePatterns,
    }),
  });
  expect(updated).toBeInstanceOf(DirectoryCopyOverride);
});

test('CLI answers pass exclusions to the directory-copy factory', async () => {
  const {factory} = context();
  jest
    .spyOn(FileSearch, 'findManifest')
    .mockResolvedValue(path.resolve('overrides.json'));
  jest.spyOn(inquirer, 'prompt').mockResolvedValue({
    type: 'directoryCopy',
    baseDirectory: 'upstream',
    issue: 123,
    excludePatternsJson: JSON.stringify(excludePatterns),
  });
  const answers = await promptForOverrideDetails();
  const override = await overrideFromDetails(
    path.resolve('mirror'),
    answers,
    factory,
  );
  expect(override.serialize()).toMatchObject({excludePatterns});
});
