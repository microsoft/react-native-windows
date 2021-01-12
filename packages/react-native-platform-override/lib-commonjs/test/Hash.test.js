"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const Hash_1 = require("../Hash");
const MockFileRepository_1 = require("./MockFileRepository");
const isutf8_1 = require("isutf8");
test('hashContent - Same String', () => {
    expect(Hash_1.hashContent('a')).toBe(Hash_1.hashContent('a'));
});
test('hashContent - Different Strings', () => {
    expect(Hash_1.hashContent('a')).not.toBe(Hash_1.hashContent('b'));
});
test('hashContent - Insensitive to Line Ending Type', () => {
    expect(Hash_1.hashContent('a\r\n')).toBe(Hash_1.hashContent('a\n'));
});
test('Hasher - Same between strings and string buffers', () => {
    expect(Hash_1.hashContent('a\r\n')).toBe(Hash_1.hashContent(Buffer.from('a\n')));
});
test('hashContent - Repeated Different Line Endings', () => {
    expect(Hash_1.hashContent('a\r\n\r\n')).toBe(Hash_1.hashContent('a\n\n'));
});
test('hashContent - Line Endings are Visible', () => {
    expect(Hash_1.hashContent('a')).not.toBe(Hash_1.hashContent('a\n\n'));
});
test('hashContent - Different Binary (Not UTF8)', () => {
    // Characters sequences starting with > 0xF4 are not valid UTF8
    const binary1 = Buffer.from(new Int8Array([0xf8, 1, 42]));
    const binary2 = Buffer.from(new Int8Array([0xff, 1, 42]));
    // Make sure we really are testing invalid strings
    expect(isutf8_1.default(binary1)).toBe(false);
    expect(isutf8_1.default(binary2)).toBe(false);
    expect(Hash_1.hashContent(Buffer.from(binary1))).not.toBe(Hash_1.hashContent(Buffer.from(binary2)));
});
test('Hasher - Same as hashContent', () => {
    expect(new Hash_1.Hasher().feedContent(Buffer.from('abc')).digest()).toBe(Hash_1.hashContent('abc'));
});
test('Hasher - Multiple Buffers', () => {
    const hash1 = new Hash_1.Hasher()
        .feedContent(Buffer.from('a'))
        .feedContent(Buffer.from('b'))
        .digest();
    const hash2 = new Hash_1.Hasher().feedContent(Buffer.from('ab')).digest();
    expect(hash1).toBe(hash2);
});
test('hashFileOrDirectory - No File', async () => {
    const fileRepo = new MockFileRepository_1.default([
        { filename: 'a/b.txt', content: 'Hello World!' },
    ]);
    expect(await Hash_1.hashFileOrDirectory('bope', fileRepo)).toBeNull();
});
test('hashFileOrDirectory - File', async () => {
    const fileRepo = new MockFileRepository_1.default([
        { filename: 'a/b.txt', content: 'Hello World!' },
    ]);
    expect(await Hash_1.hashFileOrDirectory('a/b.txt', fileRepo)).toBe(Hash_1.hashContent('Hello World!'));
});
test('hashFileOrDirectory - Directory', async () => {
    const fileRepo1 = new MockFileRepository_1.default([
        { filename: 'a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'a/b.txt', content: 'Hello World!' },
        { filename: 'b/a.txt', content: 'Ignore Me' },
    ]);
    const fileRepo2 = new MockFileRepository_1.default([
        { filename: 'a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'a/b.txt', content: 'Goodbye World!' },
        { filename: 'b/a.txt', content: 'Ignore Me' },
    ]);
    expect(await Hash_1.hashFileOrDirectory('a', fileRepo1)).not.toBe(await Hash_1.hashFileOrDirectory('a', fileRepo2));
});
test('hashFileOrDirectory - Directory Unsorted', async () => {
    const fileRepo1 = new MockFileRepository_1.default([
        { filename: 'a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'a/b.txt', content: 'Hello World!' },
        { filename: 'b/a.txt', content: 'Ignore Me' },
    ]);
    const fileRepo2 = new MockFileRepository_1.default([
        { filename: 'a/b.txt', content: 'Hello World!' },
        { filename: 'a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'b/a.txt', content: 'Ignore Me' },
    ]);
    expect(await Hash_1.hashFileOrDirectory('a', fileRepo1)).toBe(await Hash_1.hashFileOrDirectory('a', fileRepo2));
});
test('hashFileOrDirectory - Directory Rename', async () => {
    const fileRepo1 = new MockFileRepository_1.default([
        { filename: 'a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'a/b.txt', content: 'Hello World!' },
        { filename: 'b/a.txt', content: 'Ignore Me' },
    ]);
    const fileRepo2 = new MockFileRepository_1.default([
        { filename: 'a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'a/a.txt', content: 'Hello World!' },
        { filename: 'b/a.txt', content: 'Ignore Me' },
    ]);
    expect(await Hash_1.hashFileOrDirectory('a', fileRepo1)).not.toBe(await Hash_1.hashFileOrDirectory('a', fileRepo2));
});
test('hashFileOrDirectory - Different Parents', async () => {
    const fileRepo1 = new MockFileRepository_1.default([
        { filename: 'a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'a/b.txt', content: 'Hello World!' },
        { filename: 'b/a.txt', content: 'Ignore Me' },
    ]);
    const fileRepo2 = new MockFileRepository_1.default([
        { filename: 'src/a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'src/a/b.txt', content: 'Hello World!' },
        { filename: 'src/b/a.txt', content: 'Ignore Me' },
    ]);
    expect(await Hash_1.hashFileOrDirectory('a', fileRepo1)).toBe(await Hash_1.hashFileOrDirectory('src/a', fileRepo2));
});
test('hashFileOrDirectory - Empty Directory', async () => {
    const fileRepo = new MockFileRepository_1.default([
        { filename: 'a/c.txt', content: 'Lorem Ipsum' },
        { filename: 'a/b.txt', content: 'Hello World!' },
        { filename: 'b/a.txt', content: 'Ignore Me' },
    ], [{ dirname: 'empty1' }, { dirname: 'empty2' }]);
    const hash1 = await Hash_1.hashFileOrDirectory('empty1', fileRepo);
    const hash2 = await Hash_1.hashFileOrDirectory('empty2', fileRepo);
    expect(hash1).not.toBeNull();
    expect(hash2).not.toBeNull();
    expect(hash1).toEqual(hash2);
});
//# sourceMappingURL=Hash.test.js.map