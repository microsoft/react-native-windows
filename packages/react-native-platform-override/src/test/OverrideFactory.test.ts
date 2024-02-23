/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// cspell:ignoreRegExp /content: '\w+'/

import MockFileRepository, {
  MockReactFileRepository,
} from './MockFileRepository';

import {OverrideFactoryImpl} from '../OverrideFactory';
import {normalizePath} from '../PathUtils';

const factory = new OverrideFactoryImpl(
  new MockReactFileRepository([
    {filename: 'foo.js', content: 'ABCD'},
    {filename: 'bar/test.js', content: 'I live in a directory'},
  ]),
  new MockFileRepository([
    {filename: 'foo.windows.js', content: 'DCBA'},
    {filename: 'src-win/bar/test.windows.js', content: 'I live in a folder'},
  ]),
);

test('createPlatformOverride', async () => {
  const override = await factory.createPlatformOverride('foo.windows.js');
  expect(override.name()).toBe('foo.windows.js');
  expect(override.serialize().type).toBe('platform');
  expect(override.serialize().file).toBe('foo.windows.js');
});

test('createCopyOverride', async () => {
  const override = await factory.createCopyOverride(
    'foo.windows.js',
    'foo.js',
    1245,
  );
  expect(override.name()).toBe('foo.windows.js');

  expect(override.serialize().type).toBe('copy');
  expect(override.serialize().file).toBe('foo.windows.js');
  expect(override.serialize().baseFile).toBe('foo.js');
  expect(override.serialize().issue).toBe(1245);
});

test('createDerivedOverride', async () => {
  const override = await factory.createDerivedOverride(
    'foo.windows.js',
    'foo.js',
    1245,
  );
  expect(override.name()).toBe('foo.windows.js');

  expect(override.serialize().type).toBe('derived');
  expect(override.serialize().file).toBe('foo.windows.js');
  expect(override.serialize().baseFile).toBe('foo.js');
  expect(override.serialize().issue).toBe(1245);
});

test('createDerivedOverrideNoIssue', async () => {
  const override = await factory.createDerivedOverride(
    'foo.windows.js',
    'foo.js',
  );
  expect(override.name()).toBe('foo.windows.js');

  expect(override.serialize().type).toBe('derived');
  expect(override.serialize().file).toBe('foo.windows.js');
  expect(override.serialize().baseFile).toBe('foo.js');
  expect(override.serialize().issue).toBeUndefined();
});

test('createPatchOverride', async () => {
  const override = await factory.createPatchOverride(
    'foo.windows.js',
    'foo.js',
    1245,
  );
  expect(override.name()).toBe('foo.windows.js');

  expect(override.serialize().type).toBe('patch');
  expect(override.serialize().file).toBe('foo.windows.js');
  expect(override.serialize().baseFile).toBe('foo.js');
  expect(override.serialize().issue).toBe(1245);
});

test('createDirectoryCopyOverride', async () => {
  const override = await factory.createDirectoryCopyOverride(
    'src-win/bar',
    'bar',
    1245,
  );
  expect(override.name()).toBe(normalizePath('src-win/bar'));

  expect(override.serialize().type).toBe('copy');
  expect(override.serialize().directory).toBe('src-win/bar');
  expect(override.serialize().baseDirectory).toBe('bar');
  expect(override.serialize().issue).toBe(1245);
});

type ReturnsPromise = (...args: any[]) => Promise<any>;
type TestCall<T extends ReturnsPromise> = [string, T, Parameters<T>];
function testCall<T extends ReturnsPromise>(
  call: T,
  params: Parameters<T>,
): TestCall<T> {
  return [call.name, call, params];
}

test.each(<TestCall<ReturnsPromise>[]>[
  testCall(factory.createPlatformOverride, ['nope.windows.js']),
  testCall(factory.createCopyOverride, ['nope.windows.js', 'foo.js', 1245]),
  testCall(factory.createDerivedOverride, ['nope.windows.js', 'foo.js', 1245]),
  testCall(factory.createPatchOverride, ['nope.windows.js', 'foo.js', 1245]),
  testCall(factory.createDirectoryCopyOverride, ['nope', 'bar', 1245]),
])('%s - No Override', (_, fn, params) => {
  // eslint-disable-next-line @typescript-eslint/no-floating-promises
  expect(fn.apply(params)).rejects.toThrow();
});

test.each(<TestCall<ReturnsPromise>[]>[
  testCall(factory.createCopyOverride, ['foo.windows.js', 'nope.js', 1245]),
  testCall(factory.createDerivedOverride, ['foo.windows.js', 'nope.js', 1245]),
  testCall(factory.createPatchOverride, ['foo.windows.js', 'nope.js', 1245]),
  testCall(factory.createDirectoryCopyOverride, ['src-win/bar', 'nope', 1245]),
])('%s - No Base', (_, fn, params) => {
  // eslint-disable-next-line @typescript-eslint/no-floating-promises
  expect(fn.apply(params)).rejects.toThrow();
});
