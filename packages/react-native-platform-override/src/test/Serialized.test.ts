/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Serialized from '../Serialized';
import * as path from 'path';

test('Empty Manifest Invalid', () => {
  expect(() => Serialized.parseManifest('{}')).toThrow();
});

test('Well Formed No Overrides', () => {
  expect(Serialized.parseManifest('{"overrides": []}')).toEqual({
    overrides: [],
  });
});

test('Well Formed Platform', () => {
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

test('Well Formed Patch', () => {
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

test('Well Formed Derived', () => {
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

test('Well Formed Copy', () => {
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

test('Fixme Allowed As Issue', () => {
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

test('Path Should Use Platform Slashes', () => {
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

test('Issue Must Be Present For Patch', () => {
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

test('Issue Must Be Present For Copy', () => {
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

test('Issue Cannot Be Arbitrary String', () => {
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

test('Well Formed Patch Must Have Metadata', () => {
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

test('Well Formed Derived Must Have Metadata', () => {
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

test('Well Formed Copy Must Have Metadata', () => {
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
