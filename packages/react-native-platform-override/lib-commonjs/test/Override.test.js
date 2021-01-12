"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const path = require("path");
const Override_1 = require("../Override");
function testCase(type, ovrClass, args) {
    return [type, ovrClass, args];
}
const platformOverride = testCase('Platform', Override_1.PlatformOverride, {
    file: 'foo.windows.js',
});
const copyOverride = testCase('Copy', Override_1.CopyOverride, {
    file: 'foo.windows.js',
    baseFile: 'foo.js',
    baseVersion: '0.62.2',
    baseHash: 'abcde',
    issue: 1234,
});
const derivedOverride = testCase('Derived', Override_1.DerivedOverride, {
    file: 'foo.windows.js',
    baseFile: 'foo.js',
    baseVersion: '0.62.2',
    baseHash: 'abcde',
    issue: 1234,
});
const patchOverride = testCase('Patch', Override_1.PatchOverride, {
    file: 'foo.windows.js',
    baseFile: 'foo.js',
    baseVersion: '0.62.2',
    baseHash: 'abcde',
    issue: 1234,
});
const directoryCopyOverride = testCase('Directory Copy', Override_1.DirectoryCopyOverride, {
    directory: 'src/foo',
    baseDirectory: 'foo',
    baseVersion: '0.62.2',
    baseHash: 'abcde',
    issue: 1234,
});
const overrides = [
    platformOverride,
    copyOverride,
    derivedOverride,
    patchOverride,
    directoryCopyOverride,
];
const fileOverrides = [
    platformOverride,
    copyOverride,
    derivedOverride,
    patchOverride,
];
const baseFileOverrides = [
    copyOverride,
    derivedOverride,
    patchOverride,
];
const directoryOverrides = [
    directoryCopyOverride,
];
test.each(fileOverrides)('name - %s', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { file: 'bar.windows.js' }));
    expect(override.name()).toBe('bar.windows.js');
});
test.each(directoryOverrides)('name - %s', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { directory: 'src/foo/abc' }));
    expect(override.name()).toBe(path.normalize('src/foo/abc'));
});
test.each(fileOverrides)('includesFile - Has File (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { file: 'bar.windows.js' }));
    expect(override.includesFile('bar.windows.js')).toBe(true);
});
test.each(directoryOverrides)('includesFile - Has File (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { directory: 'src/foo/abc' }));
    expect(override.includesFile('src/foo/abc/d.txt')).toBe(true);
});
test.each(fileOverrides)('includesFile - No File (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { file: 'bar.windows.js' }));
    expect(override.includesFile('foo.windows.js')).toBe(false);
});
test.each(directoryOverrides)('includesFile - No File (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { directory: 'src/foo/abc' }));
    expect(override.includesFile('src/foo/abcd/e.txt')).toBe(false);
});
test.each(fileOverrides)('includesFile - Backslash (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { file: 'foo/bar.windows.js' }));
    expect(override.includesFile('foo\\bar.windows.js')).toBe(true);
});
test.each(directoryOverrides)('includesFile - Backslash (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { directory: 'src/foo/abc' }));
    expect(override.includesFile('src\\foo\\abc\\d.txt')).toBe(true);
});
test.each(fileOverrides)('includesFile - Slash (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { file: 'foo\\bar.windows.js' }));
    expect(override.includesFile('foo/bar.windows.js')).toBe(true);
});
test.each(directoryOverrides)('includesFile - Slash (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { directory: 'src/foo/abc' }));
    expect(override.includesFile('src/foo/abc/d.txt')).toBe(true);
});
test.each(overrides)('serialize - Roundtrip (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(args);
    expect(Override_1.deserializeOverride(override.serialize())).toEqual(override);
});
test.each(fileOverrides)('serialize - Override Unix Path (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { file: 'path\\to\\bar.windows.js' }));
    const serialized = override.serialize();
    expect(serialized.file).toEqual('path/to/bar.windows.js');
});
test.each(directoryOverrides)('serialize - Override Unix Path (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { directory: 'a\\b\\c' }));
    const serialized = override.serialize();
    expect(serialized.directory).toEqual('a/b/c');
});
test.each(baseFileOverrides)('serialize - Base Unix Path (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { baseFile: 'path\\to\\bar.js' }));
    const serialized = override.serialize();
    expect(serialized.baseFile).toEqual('path/to/bar.js');
});
test.each(directoryOverrides)('serialize - Base Unix Path (%s)', (_, ovrClass, args) => {
    const override = new ovrClass(Object.assign(Object.assign({}, args), { baseDirectory: 'a\\b\\c' }));
    const serialized = override.serialize();
    expect(serialized.baseDirectory).toEqual('a/b/c');
});
//# sourceMappingURL=Override.test.js.map