/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {
  MockOverrideFileRepository,
  MockReactFileRepository,
} from './MockFileRepository';

import {OverrideFactoryImpl} from '../OverrideFactory';

const factory = new OverrideFactoryImpl(
  new MockReactFileRepository([{filename: 'foo.js', content: 'ABCD'}]),
  new MockOverrideFileRepository([
    {filename: 'foo.windows.js', content: 'DCBA'},
  ]),
);

test('createPlatformOverride', async () => {
  const override = await factory.createPlatformOverride('foo.windows.js');
  expect(override.name()).toBe('foo.windows.js');
  expect(override.serialize().type).toBe('platform');
});

test('createCopyOverride', async () => {
  const override = await factory.createCopyOverride(
    'foo.windows.js',
    'foo.js',
    1245,
  );
  expect(override.name()).toBe('foo.windows.js');

  expect(override.serialize().type).toBe('copy');
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
  expect(override.serialize().baseFile).toBe('foo.js');
  expect(override.serialize().issue).toBe(1245);
});

type ReturnsPromise = (...args: any[]) => Promise<any>;
type TestCall<T extends ReturnsPromise> = [T, Parameters<T>];
function testCall<T extends ReturnsPromise>(
  call: T,
  params: Parameters<T>,
): TestCall<T> {
  return [call, params];
}

test.each(<TestCall<ReturnsPromise>[]>[
  testCall(factory.createPlatformOverride, ['nope.windows.js']),
  testCall(factory.createCopyOverride, ['nope.windows.js', 'foo.js', 1245]),
  testCall(factory.createDerivedOverride, ['nope.windows.js', 'foo.js', 1245]),
  testCall(factory.createPatchOverride, ['nope.windows.js', 'foo.js', 1245]),
])('%p No Override File', (fn, params) => {
  // @ts-ignore Typings don't know about rejects
  expect(fn.apply(params)).rejects.toThrow();
});

test.each(<TestCall<ReturnsPromise>[]>[
  testCall(factory.createCopyOverride, ['foo.windows.js', 'nope.js', 1245]),
  testCall(factory.createDerivedOverride, ['foo.windows.js', 'nope.js', 1245]),
  testCall(factory.createPatchOverride, ['foo.windows.js', 'nope.js', 1245]),
])('%p No Base File', (fn, params) => {
  // @ts-ignore Typings don't know about rejects
  expect(fn.apply(params)).rejects.toThrow();
});
