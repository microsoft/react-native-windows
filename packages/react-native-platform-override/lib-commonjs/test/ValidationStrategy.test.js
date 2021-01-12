"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const MockFileRepository_1 = require("./MockFileRepository");
const Hash_1 = require("../Hash");
const ValidationStrategy_1 = require("../ValidationStrategy");
const reactRepo = new MockFileRepository_1.MockReactFileRepository([
    { filename: 'foo.js', content: 'ABCD' },
    { filename: 'RNTester/index.js', content: '1234' },
    { filename: 'RNTester/abs.js', content: '12eee34' },
]);
const overrideRepo = new MockFileRepository_1.MockWritableFileRepository([
    { filename: 'foo.windows.js', content: 'DCBA' },
    { filename: 'bar/RNTester/index.js', content: '1234' },
    { filename: 'bar/RNTester/abs.js', content: '12eee34' },
    { filename: 'bar/RNTester/def.windowsjs', content: 'Extra' },
]);
test('overrideFileExists - File Exists', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideFileExists('foo.windows.js').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('overrideFileExists - File Does Not Exist', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideFileExists('nope.windows.js').validate(overrideRepo, reactRepo)).toEqual([{ type: 'overrideNotFound', overrideName: 'nope.windows.js' }]);
});
test('overrideFileExists - Is Directory', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideFileExists('bar').validate(overrideRepo, reactRepo)).toEqual([{ type: 'expectedFile', overrideName: 'bar' }]);
});
test('overrideDirectoryExists - Directory Exists', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDirectoryExists('bar/RNTester').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('overrideDirectoryExists - Directory Does Not Exist', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDirectoryExists('nope').validate(overrideRepo, reactRepo)).toEqual([{ type: 'overrideNotFound', overrideName: 'nope' }]);
});
test('overrideDirectoryExists - Is File', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDirectoryExists('foo.windows.js').validate(overrideRepo, reactRepo)).toEqual([{ type: 'expectedDirectory', overrideName: 'foo.windows.js' }]);
});
test('baseFileExists - File Exists', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseFileExists('foo.windows.js', 'foo.js').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('baseFileExists - File Does Not Exist', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseFileExists('foo.windows.js', 'nope.js').validate(overrideRepo, reactRepo)).toEqual([{ type: 'baseNotFound', overrideName: 'foo.windows.js' }]);
});
test('baseFileExists - Is Directory', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseFileExists('foo.windows.js', 'RNTester').validate(overrideRepo, reactRepo)).toEqual([{ type: 'expectedFile', overrideName: 'foo.windows.js' }]);
});
test('baseDirectoryExists - Directory Exists', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseDirectoryExists('bar/RNTester', 'RNTester').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('baseDirectoryExists - Directory Does Not Exist', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseDirectoryExists('bar/RNTester', 'nope').validate(overrideRepo, reactRepo)).toEqual([{ type: 'baseNotFound', overrideName: 'bar/RNTester' }]);
});
test('baseDirectoryExists - Is File', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseDirectoryExists('bar/RNTester', 'foo.js').validate(overrideRepo, reactRepo)).toEqual([{ type: 'expectedDirectory', overrideName: 'bar/RNTester' }]);
});
test('baseUpToDate - Up To Date File', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseUpToDate('foo.windows.js', 'foo.js', Hash_1.hashContent('ABCD')).validate(overrideRepo, reactRepo)).toEqual([]);
});
test('baseUpToDate - Out Of Date File', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseUpToDate('foo.windows.js', 'foo.js', Hash_1.hashContent('Old content')).validate(overrideRepo, reactRepo)).toEqual([{ type: 'outOfDate', overrideName: 'foo.windows.js' }]);
});
test('baseUpToDate - Up To Date Directory', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseUpToDate('bar/RNTester', 'RNTester', (await Hash_1.hashFileOrDirectory('RNTester', reactRepo))).validate(overrideRepo, reactRepo)).toEqual([]);
});
test('baseUpToDate - Out Of Date Directory', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseUpToDate('bar/RNTester', 'RNTester', 'OldHash').validate(overrideRepo, reactRepo)).toEqual([{ type: 'outOfDate', overrideName: 'bar/RNTester' }]);
});
test('baseUpToDate - No Override', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseUpToDate('nope.windows.js', 'foo.js', Hash_1.hashContent('ABCD')).validate(overrideRepo, reactRepo)).toEqual([]);
});
test('baseUpToDate - No Base', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.baseUpToDate('foo.windows.js', 'nope.js', Hash_1.hashContent('ABCD')).validate(overrideRepo, reactRepo)).toEqual([]);
});
test('overrideCopyOfBase - File Is Copy', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase('foo.windows.js', 'foo.js').validate(new MockFileRepository_1.MockWritableFileRepository([
        { filename: 'foo.windows.js', content: 'ABCD' },
    ]), new MockFileRepository_1.MockReactFileRepository([{ filename: 'foo.js', content: 'ABCD' }]))).toEqual([]);
});
test('overrideCopyOfBase - Directory Is Copy', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase('src/foo', 'foo').validate(new MockFileRepository_1.MockWritableFileRepository([
        { filename: 'src/foo/a.js', content: 'ABCD' },
        { filename: 'src/foo/b.js', content: 'EFGH' },
    ]), new MockFileRepository_1.MockReactFileRepository([
        { filename: 'foo/b.js', content: 'EFGH' },
        { filename: 'foo/a.js', content: 'ABCD' },
    ]))).toEqual([]);
});
test('overrideCopyOfBase - Different Line Endings', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase('foo.windows.js', 'foo.js').validate(new MockFileRepository_1.MockWritableFileRepository([
        { filename: 'foo.windows.js', content: 'ABCD\n' },
    ]), new MockFileRepository_1.MockReactFileRepository([{ filename: 'foo.js', content: 'ABCD\r\n' }]))).toEqual([]);
});
test('overrideCopyOfBase - File Not A Copy', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase('foo.windows.js', 'foo.js').validate(overrideRepo, reactRepo)).toEqual([
        { type: 'overrideDifferentFromBase', overrideName: 'foo.windows.js' },
    ]);
});
test('overrideCopyOfBase - Directory Not A Copy', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase('src/foo', 'foo').validate(new MockFileRepository_1.MockWritableFileRepository([
        { filename: 'src/foo/a.js', content: 'ABCDDCBA' },
        { filename: 'src/foo/b.js', content: 'EFGH' },
    ]), new MockFileRepository_1.MockReactFileRepository([
        { filename: 'foo/b.js', content: 'EFGH' },
        { filename: 'foo/a.js', content: 'ABCD' },
    ]))).toEqual([{ type: 'overrideDifferentFromBase', overrideName: 'src/foo' }]);
});
test('overrideCopyOfBase - Directory Has Extra File', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase('bar/RNTester', 'RNTester').validate(overrideRepo, reactRepo)).toEqual([
        { type: 'overrideDifferentFromBase', overrideName: 'bar/RNTester' },
    ]);
});
test('overrideCopyOfBase - No Override', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase('nope.js', 'foo.js').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('overrideCopyOfBase - No Base', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideCopyOfBase('foo.windows.js', 'nope.js').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('overrideDifferentFromBase - File Is Copy', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase('foo.windows.js', 'foo.js').validate(new MockFileRepository_1.MockWritableFileRepository([
        { filename: 'foo.windows.js', content: 'ABCD' },
    ]), new MockFileRepository_1.MockReactFileRepository([{ filename: 'foo.js', content: 'ABCD' }]))).toEqual([{ type: 'overrideSameAsBase', overrideName: 'foo.windows.js' }]);
});
test('overrideDifferentFromBase - Directory Is Copy', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase('src/foo', 'foo').validate(new MockFileRepository_1.MockWritableFileRepository([
        { filename: 'src/foo/a.js', content: 'ABCD' },
        { filename: 'src/foo/b.js', content: 'EFGH' },
    ]), new MockFileRepository_1.MockReactFileRepository([
        { filename: 'foo/b.js', content: 'EFGH' },
        { filename: 'foo/a.js', content: 'ABCD' },
    ]))).toEqual([{ type: 'overrideSameAsBase', overrideName: 'src/foo' }]);
});
test('overrideDifferentFromBase - Different Line Endings', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase('foo.windows.js', 'foo.js').validate(new MockFileRepository_1.MockWritableFileRepository([
        { filename: 'foo.windows.js', content: 'ABCD\n' },
    ]), new MockFileRepository_1.MockReactFileRepository([{ filename: 'foo.js', content: 'ABCD\r\n' }]))).toEqual([{ type: 'overrideSameAsBase', overrideName: 'foo.windows.js' }]);
});
test('overrideDifferentFromBase - File Not A Copy', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase('foo.windows.js', 'foo.js').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('overrideDifferentFromBase - Directory Not A Copy', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase('src/foo', 'foo').validate(new MockFileRepository_1.MockWritableFileRepository([
        { filename: 'src/foo/a.js', content: 'ABCDDCBA' },
        { filename: 'src/foo/b.js', content: 'EFGH' },
    ]), new MockFileRepository_1.MockReactFileRepository([
        { filename: 'foo/b.js', content: 'EFGH' },
        { filename: 'foo/a.js', content: 'ABCD' },
    ]))).toEqual([]);
});
test('overrideDifferentFromBase - Directory Has Extra File', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase('bar/RNTester', 'RNTester').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('overrideDifferentFromBase - No Override', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase('nope.js', 'foo.js').validate(overrideRepo, reactRepo)).toEqual([]);
});
test('overrideDifferentFromBase - No Base', async () => {
    expect(await ValidationStrategy_1.ValidationStrategies.overrideDifferentFromBase('foo.windows.js', 'nope.js').validate(overrideRepo, reactRepo)).toEqual([]);
});
//# sourceMappingURL=ValidationStrategy.test.js.map