/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Hasher, hashContent, hashFileOrDirectory} from '../Hash';
import MockFileRepository from './MockFileRepository';
import isutf8 from 'isutf8';

test('hashContent - Same String', () => {
  expect(hashContent('a')).toBe(hashContent('a'));
});

test('hashContent - Different Strings', () => {
  expect(hashContent('a')).not.toBe(hashContent('b'));
});

test('hashContent - Insensitive to Line Ending Type', () => {
  expect(hashContent('a\r\n')).toBe(hashContent('a\n'));
});

test('Hasher - Same between strings and string buffers', () => {
  expect(hashContent('a\r\n')).toBe(hashContent(Buffer.from('a\n')));
});

test('hashContent - Repeated Different Line Endings', () => {
  expect(hashContent('a\r\n\r\n')).toBe(hashContent('a\n\n'));
});

test('hashContent - Line Endings are Visible', () => {
  expect(hashContent('a')).not.toBe(hashContent('a\n\n'));
});

test('hashContent - Different Binary (Not UTF8)', () => {
  // Characters sequences starting with > 0xF4 are not valid UTF8
  const binary1 = Buffer.from(new Int8Array([0xf8, 1, 42]));
  const binary2 = Buffer.from(new Int8Array([0xff, 1, 42]));

  // Make sure we really are testing invalid strings
  expect(isutf8(binary1)).toBe(false);
  expect(isutf8(binary2)).toBe(false);

  expect(hashContent(Buffer.from(binary1))).not.toBe(
    hashContent(Buffer.from(binary2)),
  );
});

test('Hasher - Same as hashContent', () => {
  expect(new Hasher().feedContent(Buffer.from('abc')).digest()).toBe(
    hashContent('abc'),
  );
});

test('Hasher - Multiple Buffers', () => {
  const hash1 = new Hasher()
    .feedContent(Buffer.from('a'))
    .feedContent(Buffer.from('b'))
    .digest();

  const hash2 = new Hasher().feedContent(Buffer.from('ab')).digest();

  expect(hash1).toBe(hash2);
});

test('hashFileOrDirectory - No File', async () => {
  const fileRepo = new MockFileRepository([
    {filename: 'a/b.txt', content: 'Hello World!'},
  ]);

  expect(await hashFileOrDirectory('bope', fileRepo)).toBeNull();
});

test('hashFileOrDirectory - File', async () => {
  const fileRepo = new MockFileRepository([
    {filename: 'a/b.txt', content: 'Hello World!'},
  ]);

  expect(await hashFileOrDirectory('a/b.txt', fileRepo)).toBe(
    hashContent('Hello World!'),
  );
});

test('hashFileOrDirectory - Directory', async () => {
  const fileRepo = new MockFileRepository([
    {filename: 'a/b.txt', content: 'Hello World!'},
    {filename: 'a/c.txt', content: 'Lorem Ipsum'},
    {filename: 'b/a.txt', content: 'Ignore Me'},
  ]);

  expect(await hashFileOrDirectory('a', fileRepo)).toBe(
    new Hasher()
      .feedContent('Hello World!')
      .feedContent('Lorem Ipsum')
      .digest(),
  );
});

test('hashFileOrDirectory - Directory Unsorted', async () => {
  const fileRepo = new MockFileRepository([
    {filename: 'a/c.txt', content: 'Lorem Ipsum'},
    {filename: 'a/b.txt', content: 'Hello World!'},
    {filename: 'b/a.txt', content: 'Ignore Me'},
  ]);

  expect(await hashFileOrDirectory('a', fileRepo)).toBe(
    new Hasher()
      .feedContent('Hello World!')
      .feedContent('Lorem Ipsum')
      .digest(),
  );
});

test('hashFileOrDirectory - Empty Directory', async () => {
  const fileRepo = new MockFileRepository(
    [
      {filename: 'a/c.txt', content: 'Lorem Ipsum'},
      {filename: 'a/b.txt', content: 'Hello World!'},
      {filename: 'b/a.txt', content: 'Ignore Me'},
    ],
    [{dirname: 'empty1'}, {dirname: 'empty2'}],
  );

  const hash1 = await hashFileOrDirectory('empty1', fileRepo);
  const hash2 = await hashFileOrDirectory('empty2', fileRepo);

  expect(hash1).not.toBeNull();
  expect(hash2).not.toBeNull();
  expect(hash1).toEqual(hash2);
});
