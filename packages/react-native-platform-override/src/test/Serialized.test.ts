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
