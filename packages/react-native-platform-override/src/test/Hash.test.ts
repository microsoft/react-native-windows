/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Hasher, hashContent} from '../Hash';
import isutf8 from 'isutf8';

test('hashContent - Different Strings', () => {
  expect(hashContent('a')).not.toBe(hashContent('b'));
});

test('hashContent - Insensitive to Line Ending Type', () => {
  expect(hashContent('a\r\n')).toBe(hashContent('a\n'));
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
