/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import Override, {
  CopyOverride,
  DerivedOverride,
  DirectoryCopyOverride,
  PatchOverride,
  PlatformOverride,
  deserializeOverride,
} from '../Override';
import {normalizePath} from '../PathUtils';

type OverrideConstructor = new (...args: any[]) => Override;

type TestCase<T extends OverrideConstructor> = [
  string,
  T,
  ConstructorParameters<T>[0],
];

function testCase<T extends OverrideConstructor>(
  type: string,
  ovrClass: T,
  args: ConstructorParameters<T>[0],
): TestCase<T> {
  return [type, ovrClass, args];
}

const platformOverride = testCase('Platform', PlatformOverride, {
  file: 'foo.windows.js',
});

const copyOverride = testCase('Copy', CopyOverride, {
  file: 'foo.windows.js',
  baseFile: 'foo.js',
  baseVersion: '0.62.2',
  baseHash: 'abcde',
  issue: 1234,
});

const derivedOverride = testCase('Derived', DerivedOverride, {
  file: 'foo.windows.js',
  baseFile: 'foo.js',
  baseVersion: '0.62.2',
  baseHash: 'abcde',
  issue: 1234,
});

const patchOverride = testCase('Patch', PatchOverride, {
  file: 'foo.windows.js',
  baseFile: 'foo.js',
  baseVersion: '0.62.2',
  baseHash: 'abcde',
  issue: 1234,
});

const directoryCopyOverride = testCase(
  'Directory Copy',
  DirectoryCopyOverride,
  {
    directory: 'src/foo',
    baseDirectory: 'foo',
    baseVersion: '0.62.2',
    baseHash: 'abcde',
    issue: 1234,
  },
);

const overrides: TestCase<OverrideConstructor>[] = [
  platformOverride,
  copyOverride,
  derivedOverride,
  patchOverride,
  directoryCopyOverride,
];

const fileOverrides: TestCase<OverrideConstructor>[] = [
  platformOverride,
  copyOverride,
  derivedOverride,
  patchOverride,
];

const baseFileOverrides: TestCase<OverrideConstructor>[] = [
  copyOverride,
  derivedOverride,
  patchOverride,
];

const directoryOverrides: TestCase<OverrideConstructor>[] = [
  directoryCopyOverride,
];

test.each(fileOverrides)('name - %s', (_, ovrClass, args) => {
  const override = new ovrClass({...args, file: 'bar.windows.js'});
  expect(override.name()).toBe('bar.windows.js');
});

test.each(directoryOverrides)('name - %s', (_, ovrClass, args) => {
  const override = new ovrClass({...args, directory: 'src/foo/abc'});
  expect(override.name()).toBe(normalizePath('src/foo/abc'));
});

test.each(fileOverrides)(
  'includesFile - Has File (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, file: 'bar.windows.js'});
    expect(override.includesFile('bar.windows.js')).toBe(true);
  },
);

test.each(directoryOverrides)(
  'includesFile - Has File (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, directory: 'src/foo/abc'});
    expect(override.includesFile('src/foo/abc/d.txt')).toBe(true);
  },
);

test.each(fileOverrides)('includesFile - No File (%s)', (_, ovrClass, args) => {
  const override = new ovrClass({...args, file: 'bar.windows.js'});
  expect(override.includesFile('foo.windows.js')).toBe(false);
});

test.each(directoryOverrides)(
  'includesFile - No File (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, directory: 'src/foo/abc'});
    expect(override.includesFile('src/foo/abcd/e.txt')).toBe(false);
  },
);

test.each(fileOverrides)(
  'includesFile - Backslash (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, file: 'foo/bar.windows.js'});
    expect(override.includesFile('foo\\bar.windows.js')).toBe(true);
  },
);

test.each(directoryOverrides)(
  'includesFile - Backslash (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, directory: 'src/foo/abc'});
    expect(override.includesFile('src\\foo\\abc\\d.txt')).toBe(true);
  },
);

test.each(fileOverrides)('includesFile - Slash (%s)', (_, ovrClass, args) => {
  const override = new ovrClass({...args, file: 'foo\\bar.windows.js'});
  expect(override.includesFile('foo/bar.windows.js')).toBe(true);
});

test.each(directoryOverrides)(
  'includesFile - Slash (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, directory: 'src/foo/abc'});
    expect(override.includesFile('src/foo/abc/d.txt')).toBe(true);
  },
);

test.each(overrides)('serialize - Roundtrip (%s)', (_, ovrClass, args) => {
  const override = new ovrClass(args);
  expect(deserializeOverride(override.serialize())).toEqual(override);
});

test.each(fileOverrides)(
  'serialize - Override Unix Path (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, file: 'path\\to\\bar.windows.js'});
    const serialized = override.serialize() as any;
    expect(serialized.file).toEqual('path/to/bar.windows.js');
  },
);

test.each(directoryOverrides)(
  'serialize - Override Unix Path (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, directory: 'a\\b\\c'});
    const serialized = override.serialize() as any;
    expect(serialized.directory).toEqual('a/b/c');
  },
);

test.each(baseFileOverrides)(
  'serialize - Base Unix Path (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, baseFile: 'path\\to\\bar.js'});
    const serialized = override.serialize() as any;
    expect(serialized.baseFile).toEqual('path/to/bar.js');
  },
);

test.each(directoryOverrides)(
  'serialize - Base Unix Path (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, baseDirectory: 'a\\b\\c'});
    const serialized = override.serialize() as any;
    expect(serialized.baseDirectory).toEqual('a/b/c');
  },
);

test.each(baseFileOverrides)(
  'serialize - Default baseVersion Matches (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, baseFile: 'path\\to\\bar.js'});
    const serialized = override.serialize({
      defaultBaseVersion: '0.62.2',
    }) as any;
    expect(serialized.baseVersion).toBeUndefined();
  },
);

test.each(directoryOverrides)(
  'serialize - Default baseVersion Matches (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, baseDirectory: 'a\\b\\c'});
    const serialized = override.serialize({
      defaultBaseVersion: '0.62.2',
    }) as any;
    expect(serialized.baseVersion).toBeUndefined();
  },
);

test.each(baseFileOverrides)(
  'serialize - Default baseVersion Mismatch (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, baseFile: 'path\\to\\bar.js'});
    const serialized = override.serialize({
      defaultBaseVersion: '0.60.0',
    }) as any;
    expect(serialized.baseVersion).toBe('0.62.2');
  },
);

test.each(directoryOverrides)(
  'serialize - Default baseVersion Mismatch (%s)',
  (_, ovrClass, args) => {
    const override = new ovrClass({...args, baseDirectory: 'a\\b\\c'});
    const serialized = override.serialize({
      defaultBaseVersion: '0.60.0',
    }) as any;
    expect(serialized.baseVersion).toBe('0.62.2');
  },
);
