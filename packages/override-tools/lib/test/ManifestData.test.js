/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as ManifestData from '../ManifestData';
import * as path from 'path';
test('EmptyManifestInvalid', () => {
    expect(() => ManifestData.parse('{}')).toThrow();
});
test('WellFormedNoOverrides', () => {
    expect(ManifestData.parse('{"overrides": []}')).toEqual({ overrides: [] });
});
test('WellFormedPlatform', () => {
    const manifest = {
        overrides: [
            {
                type: 'platform',
                file: 'foo.win32.js',
            },
        ],
    };
    expect(ManifestData.parse(JSON.stringify(manifest))).toEqual(manifest);
});
test('WellFormedPatch', () => {
    const manifest = {
        overrides: [
            {
                type: 'patch',
                file: 'foo.win32.js',
                baseFile: 'foo.js',
                baseVersion: '0.61.5',
                baseHash: 'AAAABBBB',
                issue: 4567,
            },
        ],
    };
    expect(ManifestData.parse(JSON.stringify(manifest))).toEqual(manifest);
});
test('WellFormedDerived', () => {
    const manifest = {
        overrides: [
            {
                type: 'derived',
                file: 'foo.win32.js',
                baseFile: 'foo.js',
                baseVersion: '0.61.5',
                baseHash: 'AAAABBBB',
                issue: 4567,
            },
        ],
    };
    expect(ManifestData.parse(JSON.stringify(manifest))).toEqual(manifest);
});
test('WellFormedCopy', () => {
    const manifest = {
        overrides: [
            {
                type: 'copy',
                file: 'foo.win32.js',
                baseFile: 'foo.js',
                baseVersion: '0.61.5',
                baseHash: 'AAAABBBB',
                issue: 4567,
            },
        ],
    };
    expect(ManifestData.parse(JSON.stringify(manifest))).toEqual(manifest);
});
test('FixmeAllowedAsIssue', () => {
    const manifest = {
        overrides: [
            {
                type: 'patch',
                file: 'foo.win32.js',
                baseFile: 'foo.js',
                baseVersion: '0.61.5',
                baseHash: 'AAAABBBB',
                issue: 'LEGACY_FIXME',
            },
        ],
    };
    expect(ManifestData.parse(JSON.stringify(manifest))).toEqual(manifest);
});
test('PathShouldUsePlatformSlashes', () => {
    const manifest = {
        overrides: [
            {
                type: 'patch',
                file: 'foo/foo.win32.js',
                baseFile: 'bar\\foo.js',
                baseVersion: '0.61.5',
                baseHash: 'AAAABBBB',
                issue: 2345,
            },
        ],
    };
    const parsed = ManifestData.parse(JSON.stringify(manifest));
    const override = parsed.overrides[0];
    expect(override.file).toBe(`foo${path.sep}foo.win32.js`);
    expect(override.baseFile).toBe(`bar${path.sep}foo.js`);
});
test('IssueMustBePresentForPatch', () => {
    const manifest = {
        overrides: [
            {
                type: 'patch',
                file: 'foo.win32.js',
                baseFile: 'foo.js',
                baseVersion: '0.61.5',
                baseHash: 'AAAABBBB',
            },
        ],
    };
    expect(() => ManifestData.parse(JSON.stringify(manifest))).toThrow();
});
test('IssueMustBePresentForCopy', () => {
    const manifest = {
        overrides: [
            {
                type: 'copy',
                file: 'foo.win32.js',
                baseFile: 'foo.js',
                baseVersion: '0.61.5',
                baseHash: 'AAAABBBB',
            },
        ],
    };
    expect(() => ManifestData.parse(JSON.stringify(manifest))).toThrow();
});
test('IssueCannotBeArbitraryString', () => {
    const manifest = {
        overrides: [
            {
                type: 'patch',
                file: 'foo.win32.js',
                baseFile: 'foo.js',
                baseVersion: '0.61.5',
                baseHash: 'AAAABBBB',
                issue: 'bad',
            },
        ],
    };
    expect(() => ManifestData.parse(JSON.stringify(manifest))).toThrow();
});
test('WellFormedPatchMustHaveMetadata', () => {
    const manifest = {
        overrides: [
            {
                type: 'patch',
                file: 'foo.win32.js',
            },
        ],
    };
    expect(() => ManifestData.parse(JSON.stringify(manifest))).toThrow();
});
test('WellFormedDerivedMustHaveMetadata', () => {
    const manifest = {
        overrides: [
            {
                type: 'derived',
                file: 'foo.win32.js',
            },
        ],
    };
    expect(() => ManifestData.parse(JSON.stringify(manifest))).toThrow();
});
test('WellFormedCopyMustHaveMetadata', () => {
    const manifest = {
        overrides: [
            {
                type: 'copy',
                file: 'foo.win32.js',
            },
        ],
    };
    expect(() => ManifestData.parse(JSON.stringify(manifest))).toThrow();
});
//# sourceMappingURL=ManifestData.test.js.map