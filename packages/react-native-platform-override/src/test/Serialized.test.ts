/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Serialized from '../Serialized';
import * as path from 'path';

test('EmptyManifestInvalid', () => {
  expect(() => Serialized.parseManifest('{}')).toThrow();
});

test('WellFormedNoOverrides', () => {
  expect(Serialized.parseManifest('{"overrides": []}')).toEqual({
    overrides: [],
  });
});

test('WellFormedPlatform', () => {
  const manifest: Serialized.Manifest = {
    overrides: [
      {
        type: 'platform',
        file: 'foo.win32.js',
      },
    ],
  };

  expect(Serialized.parseManifest(JSON.stringify(manifest))).toEqual(manifest);
});

test('WellFormedPatch', () => {
  const manifest: Serialized.Manifest = {
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

  expect(Serialized.parseManifest(JSON.stringify(manifest))).toEqual(manifest);
});

test('WellFormedDerived', () => {
  const manifest: Serialized.Manifest = {
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

  expect(Serialized.parseManifest(JSON.stringify(manifest))).toEqual(manifest);
});

test('WellFormedCopy', () => {
  const manifest: Serialized.Manifest = {
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

  expect(Serialized.parseManifest(JSON.stringify(manifest))).toEqual(manifest);
});

test('FixmeAllowedAsIssue', () => {
  const manifest: Serialized.Manifest = {
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

  expect(Serialized.parseManifest(JSON.stringify(manifest))).toEqual(manifest);
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

  const parsed = Serialized.parseManifest(JSON.stringify(manifest));
  const override = parsed.overrides[0] as Serialized.PatchOverride;
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

  expect(() => Serialized.parseManifest(JSON.stringify(manifest))).toThrow();
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

  expect(() => Serialized.parseManifest(JSON.stringify(manifest))).toThrow();
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

  expect(() => Serialized.parseManifest(JSON.stringify(manifest))).toThrow();
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

  expect(() => Serialized.parseManifest(JSON.stringify(manifest))).toThrow();
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

  expect(() => Serialized.parseManifest(JSON.stringify(manifest))).toThrow();
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

  expect(() => Serialized.parseManifest(JSON.stringify(manifest))).toThrow();
});
