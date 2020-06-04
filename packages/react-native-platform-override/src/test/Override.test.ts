/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import Override, {
  CopyOverride,
  DerivedOverride,
  PatchOverride,
  PlatformOverride,
  deserializeOverride,
} from '../Override';

type OverrideConstructor = new (...args: any[]) => Override;

type TestCase<T extends OverrideConstructor> = [
  string,
  T,
  ConstructorParameters<T>[0]
];

function testCase<T extends OverrideConstructor>(
  type: string,
  ovrClass: T,
  args: ConstructorParameters<T>[0],
): TestCase<T> {
  return [type, ovrClass, args];
}

const platformOverride = testCase('platform', PlatformOverride, {
  file: 'foo.windows.js',
});

const copyOverride = testCase('copy', CopyOverride, {
  file: 'foo.windows.js',
  baseFile: 'foo.js',
  baseVersion: '0.62.2',
  baseHash: 'abcde',
  issue: 1234,
});

const derivedOverride = testCase('derived', DerivedOverride, {
  file: 'foo.windows.js',
  baseFile: 'foo.js',
  baseVersion: '0.62.2',
  baseHash: 'abcde',
  issue: 1234,
});

const patchOverride = testCase('pacth', PatchOverride, {
  file: 'foo.windows.js',
  baseFile: 'foo.js',
  baseVersion: '0.62.2',
  baseHash: 'abcde',
  issue: 1234,
});

const fileOverrides: TestCase<OverrideConstructor>[] = [
  platformOverride,
  copyOverride,
  derivedOverride,
  patchOverride,
];

test.each(fileOverrides)('%s Override Name', (_, ovrClass, args) => {
  const override = new ovrClass({...args, file: 'bar.windows.js'});
  expect(override.name()).toBe('bar.windows.js');
});

test.each(fileOverrides)('%s Includes Same File', (_, ovrClass, args) => {
  const override = new ovrClass({...args, file: 'bar.windows.js'});
  expect(override.includesFile('bar.windows.js')).toBe(true);
});

test.each(fileOverrides)('%s Doesnt Include File', (_, ovrClass, args) => {
  const override = new ovrClass({...args, file: 'bar.windows.js'});
  expect(override.includesFile('foo.windows.js')).toBe(false);
});

test.each(fileOverrides)('%s Includes Backslash File', (_, ovrClass, args) => {
  const override = new ovrClass({...args, file: 'foo/bar.windows.js'});
  expect(override.includesFile('foo\\bar.windows.js')).toBe(true);
});

test.each(fileOverrides)('%s Includes Slash File', (_, ovrClass, args) => {
  const override = new ovrClass({...args, file: 'foo\\bar.windows.js'});
  expect(override.includesFile('foo/bar.windows.js')).toBe(true);
});

test.each(fileOverrides)('%s Serialization Roundtrip', (_, ovrClass, args) => {
  const override = new ovrClass(args);
  expect(deserializeOverride(override.serialize())).toEqual(override);
});
