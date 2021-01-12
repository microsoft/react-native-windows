"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const path = require("path");
const MockFileRepository_1 = require("./MockFileRepository");
const OverrideFactory_1 = require("../OverrideFactory");
const factory = new OverrideFactory_1.OverrideFactoryImpl(new MockFileRepository_1.MockReactFileRepository([
    { filename: 'foo.js', content: 'ABCD' },
    { filename: 'bar/test.js', content: 'I live in a directory' },
]), new MockFileRepository_1.default([
    { filename: 'foo.windows.js', content: 'DCBA' },
    { filename: 'src/bar/test.windows.js', content: 'I live in a folder' },
]));
test('createPlatformOverride', async () => {
    const override = await factory.createPlatformOverride('foo.windows.js');
    expect(override.name()).toBe('foo.windows.js');
    expect(override.serialize().type).toBe('platform');
    expect(override.serialize().file).toBe('foo.windows.js');
});
test('createCopyOverride', async () => {
    const override = await factory.createCopyOverride('foo.windows.js', 'foo.js', 1245);
    expect(override.name()).toBe('foo.windows.js');
    expect(override.serialize().type).toBe('copy');
    expect(override.serialize().file).toBe('foo.windows.js');
    expect(override.serialize().baseFile).toBe('foo.js');
    expect(override.serialize().issue).toBe(1245);
});
test('createDerivedOverride', async () => {
    const override = await factory.createDerivedOverride('foo.windows.js', 'foo.js', 1245);
    expect(override.name()).toBe('foo.windows.js');
    expect(override.serialize().type).toBe('derived');
    expect(override.serialize().file).toBe('foo.windows.js');
    expect(override.serialize().baseFile).toBe('foo.js');
    expect(override.serialize().issue).toBe(1245);
});
test('createDerivedOverrideNoIssue', async () => {
    const override = await factory.createDerivedOverride('foo.windows.js', 'foo.js');
    expect(override.name()).toBe('foo.windows.js');
    expect(override.serialize().type).toBe('derived');
    expect(override.serialize().file).toBe('foo.windows.js');
    expect(override.serialize().baseFile).toBe('foo.js');
    expect(override.serialize().issue).toBeUndefined();
});
test('createPatchOverride', async () => {
    const override = await factory.createPatchOverride('foo.windows.js', 'foo.js', 1245);
    expect(override.name()).toBe('foo.windows.js');
    expect(override.serialize().type).toBe('patch');
    expect(override.serialize().file).toBe('foo.windows.js');
    expect(override.serialize().baseFile).toBe('foo.js');
    expect(override.serialize().issue).toBe(1245);
});
test('createDirectoryCopyOverride', async () => {
    const override = await factory.createDirectoryCopyOverride('src/bar', 'bar', 1245);
    expect(override.name()).toBe(path.normalize('src/bar'));
    expect(override.serialize().type).toBe('copy');
    expect(override.serialize().directory).toBe('src/bar');
    expect(override.serialize().baseDirectory).toBe('bar');
    expect(override.serialize().issue).toBe(1245);
});
function testCall(call, params) {
    return [call.name, call, params];
}
test.each([
    testCall(factory.createPlatformOverride, ['nope.windows.js']),
    testCall(factory.createCopyOverride, ['nope.windows.js', 'foo.js', 1245]),
    testCall(factory.createDerivedOverride, ['nope.windows.js', 'foo.js', 1245]),
    testCall(factory.createPatchOverride, ['nope.windows.js', 'foo.js', 1245]),
    testCall(factory.createDirectoryCopyOverride, ['nope', 'bar', 1245]),
])('%s - No Override', (_, fn, params) => {
    // @ts-ignore Typings don't know about rejects
    expect(fn.apply(params)).rejects.toThrow();
});
test.each([
    testCall(factory.createCopyOverride, ['foo.windows.js', 'nope.js', 1245]),
    testCall(factory.createDerivedOverride, ['foo.windows.js', 'nope.js', 1245]),
    testCall(factory.createPatchOverride, ['foo.windows.js', 'nope.js', 1245]),
    testCall(factory.createDirectoryCopyOverride, ['src/bar', 'nope', 1245]),
])('%s - No Base', (_, fn, params) => {
    // @ts-ignore Typings don't know about rejects
    expect(fn.apply(params)).rejects.toThrow();
});
//# sourceMappingURL=OverrideFactory.test.js.map