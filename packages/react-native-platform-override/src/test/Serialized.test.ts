/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Serialized from '../Serialized';

test('Empty Manifest Invalid', () => {
  expect(() => Serialized.parseManifest('{}')).toThrow();
});

test('Well Formed No Overrides', () => {
  expect(Serialized.parseManifest('{"overrides": []}')).toEqual({
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: undefined,
    overrides: [],
  });
});

test('Well Formed Platform', () => {
  const manifest: Serialized.Manifest = {
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: undefined,
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
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: undefined,
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

test('Well Formed Patch - Default Base', () => {
  const manifest: Serialized.Manifest = {
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: '0.61.5',
    overrides: [
      {
        type: 'patch',
        file: 'foo.win32.js',
        baseFile: 'foo.js',
        baseVersion: undefined,
        baseHash: 'AAAABBBB',
        issue: 4567,
      },
    ],
  };

  expect(Serialized.parseManifest(JSON.stringify(manifest))).toEqual(manifest);
});

test('Well Formed Derived', () => {
  const manifest: Serialized.Manifest = {
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: undefined,
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
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: undefined,
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

test('Well Formed Directory Copy', () => {
  const manifest: Serialized.Manifest = {
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: undefined,
    overrides: [
      {
        type: 'copy',
        directory: 'foo',
        baseDirectory: 'src/foo',
        baseVersion: '0.61.5',
        baseHash: 'AAAABBBB',
        issue: 4567,
      },
    ],
  };

  expect(Serialized.parseManifest(JSON.stringify(manifest))).toEqual(manifest);
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

test('Valid includePatterns', () => {
  const includePatterns = ['src/**', 'copies/**'];

  const manifest = {
    includePatterns,
    overrides: [],
  };

  expect(
    Serialized.parseManifest(JSON.stringify(manifest)).includePatterns,
  ).toEqual(includePatterns);
});

test('Valid excludePatterns', () => {
  const excludePatterns = ['**/*.md', '**/README.md'];

  const manifest = {
    excludePatterns,
    overrides: [],
  };

  expect(
    Serialized.parseManifest(JSON.stringify(manifest)).excludePatterns,
  ).toEqual(excludePatterns);
});
