/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {mkdirSync, mkdtempSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {
  nightlyFixupSpecs,
  parseCrnlConfig,
  readRnwWorkspaceSpecs,
  resolveBranchVersions,
  stableMinor,
} from '../specialModules/createReactNativeLibrary';
import type {SpecialModuleContext} from '../specialModules/types';
import type {NpmRegistry} from '../registries';

function fakeRegistry(versions: Record<string, string[]>): NpmRegistry {
  return {
    getVersions: async (id: string) => versions[id] ?? [],
    getTarballUrl: async () => null,
  };
}

function mctx(repoRoot: string, registry: NpmRegistry): SpecialModuleContext {
  const log = {info() {}, warn() {}, error() {}, debug() {}};
  return {
    ctx: {log} as unknown as SpecialModuleContext['ctx'],
    repoRoot,
    registry,
    npmRegistryUrl: 'https://feed/npm/',
    npmrcPath: '',
  };
}

test('stableMinor parses stable branch names, null for main', () => {
  expect(stableMinor('0.81-stable')).toBe(81);
  expect(stableMinor('0.85-stable')).toBe(85);
  expect(stableMinor('0.85')).toBe(85);
  expect(stableMinor('main')).toBeNull();
});

test('parseCrnlConfig validates required fields and applies defaults', () => {
  expect(() => parseCrnlConfig({})).toThrow(/createLibraryVersion/);
  expect(() => parseCrnlConfig({createLibraryVersion: '0.63.0'})).toThrow(
    /branches/,
  );
  const cfg = parseCrnlConfig({
    createLibraryVersion: '0.63.0',
    branches: [{name: 'main', nightly: true}, {name: '0.85-stable'}],
  });
  expect(cfg.type).toBe('turbo-module');
  expect(cfg.example).toBe('vanilla');
  expect(cfg.languages).toBe('kotlin-objc');
  expect(cfg.tools).toEqual(['eslint', 'jest', 'lefthook', 'release-it', 'vite']);
  expect(cfg.branches).toHaveLength(2);
  expect(cfg.branches[0]).toMatchObject({name: 'main', nightly: true});
});

test('parseCrnlConfig accepts a custom tools list', () => {
  const cfg = parseCrnlConfig({
    createLibraryVersion: '0.63.0',
    tools: ['eslint', 'jest'],
    branches: [{name: 'main'}],
  });
  expect(cfg.tools).toEqual(['eslint', 'jest']);
});

test('nightlyFixupSpecs rewrites the RN family and cli specs only', () => {
  const specs: Record<string, string> = {
    'react-native': '0.80.0',
    'react-native-windows': '*',
    '@react-native/new-app-screen': '0.80.0',
    '@react-native/babel-preset': '0.80.0',
    '@react-native/metro-config': '0.80.0',
    '@react-native-community/cli': '20.0.0',
    '@react-native-community/cli-platform-ios': '20.0.0',
    react: '19.0.0',
    other: '1.0.0',
  };
  nightlyFixupSpecs(specs, '0.86.0-nightly', '21.0.0');
  expect(specs['react-native']).toBe('0.86.0-nightly');
  expect(specs['@react-native/new-app-screen']).toBe('0.86.0-nightly');
  expect(specs['@react-native/babel-preset']).toBe('0.86.0-nightly');
  expect(specs['@react-native/metro-config']).toBe('0.86.0-nightly');
  expect(specs['@react-native-community/cli']).toBe('21.0.0');
  expect(specs['@react-native-community/cli-platform-ios']).toBe('21.0.0');
  // '*' peer and unrelated packages are untouched.
  expect(specs['react-native-windows']).toBe('*');
  expect(specs.react).toBe('19.0.0');
  expect(specs.other).toBe('1.0.0');
});

test('resolveBranchVersions: main reads the nightly from vnext, no rnw spec', async () => {
  const repo = mkdtempSync(join(tmpdir(), 'warm-repo-'));
  mkdirSync(join(repo, 'vnext'));
  writeFileSync(
    join(repo, 'vnext', 'package.json'),
    JSON.stringify({
      devDependencies: {'react-native': '0.86.0-nightly-x'},
      dependencies: {'@react-native-community/cli': '20.0.0'},
    }),
  );
  const v = await resolveBranchVersions(mctx(repo, fakeRegistry({})), {
    name: 'main',
    nightly: true,
  });
  expect(v.reactNative).toBe('0.86.0-nightly-x');
  expect(v.reactNativeCli).toBe('20.0.0');
  expect(v.reactNativeWindowsSpec).toBeUndefined();
  expect(v.nightly).toBe(true);
});

test('resolveBranchVersions: main derives the CLI even when reactNativeVersion is overridden', async () => {
  const repo = mkdtempSync(join(tmpdir(), 'warm-repo-'));
  mkdirSync(join(repo, 'vnext'));
  writeFileSync(
    join(repo, 'vnext', 'package.json'),
    JSON.stringify({
      devDependencies: {'react-native': '0.86.0-nightly-x'},
      dependencies: {'@react-native-community/cli': '20.0.0'},
    }),
  );
  const v = await resolveBranchVersions(mctx(repo, fakeRegistry({})), {
    name: 'main',
    nightly: true,
    reactNativeVersion: '0.86.0-nightly-pinned',
  });
  expect(v.reactNative).toBe('0.86.0-nightly-pinned');
  expect(v.reactNativeCli).toBe('20.0.0');
});

test('resolveBranchVersions: a stable branch resolves the latest stable RN and RNW', async () => {
  const reg = fakeRegistry({
    'react-native': ['0.81.0', '0.81.5', '0.81.2', '0.82.0', '0.81.9-rc.0'],
    'react-native-windows': ['0.81.0', '0.81.4', '0.81.5-preview.1'],
  });
  const v = await resolveBranchVersions(mctx('/no/repo', reg), {
    name: '0.81-stable',
  });
  expect(v.reactNative).toBe('0.81.5');
  expect(v.reactNativeWindowsSpec).toBe('0.81.4');
  expect(v.nightly).toBe(false);
});

test('resolveBranchVersions: a preview-only RNW line falls back to the newest preview', async () => {
  const reg = fakeRegistry({
    'react-native': ['0.85.0', '0.85.3'],
    'react-native-windows': ['0.85.0-preview.1', '0.85.0-preview.2'],
  });
  const v = await resolveBranchVersions(mctx('/no/repo', reg), {
    name: '0.85-stable',
  });
  expect(v.reactNative).toBe('0.85.3');
  expect(v.reactNativeWindowsSpec).toBe('0.85.0-preview.2');
});

test('resolveBranchVersions: explicit overrides win', async () => {
  const v = await resolveBranchVersions(mctx('/no/repo', fakeRegistry({})), {
    name: '0.83-stable',
    reactNativeVersion: '0.83.7',
    reactNativeWindowsSpec: '0.83.0-canary',
  });
  expect(v.reactNative).toBe('0.83.7');
  expect(v.reactNativeWindowsSpec).toBe('0.83.0-canary');
});

test('resolveBranchVersions: throws when a stable line has no versions', async () => {
  await expect(
    resolveBranchVersions(mctx('/no/repo', fakeRegistry({})), {
      name: '0.99-stable',
    }),
  ).rejects.toThrow(/could not determine a react-native version/);
});

test('readRnwWorkspaceSpecs keeps only resolvable @react-native-windows/* deps', () => {
  const repo = mkdtempSync(join(tmpdir(), 'warm-repo-'));
  mkdirSync(join(repo, 'vnext'));
  writeFileSync(
    join(repo, 'vnext', 'package.json'),
    JSON.stringify({
      dependencies: {
        '@react-native-windows/cli': '0.0.0-canary.293',
        '@react-native-community/cli': '20.0.0',
        'react-native': '0.86.0-nightly-x',
      },
      devDependencies: {
        '@react-native-windows/codegen': '0.0.0-canary.133',
        '@react-native-windows/find-dotnet-tools': '0.0.0-canary.2',
        '@rnw-scripts/just-task': 'workspace:*',
      },
    }),
  );
  expect(readRnwWorkspaceSpecs(repo)).toEqual({
    '@react-native-windows/cli': '0.0.0-canary.293',
    '@react-native-windows/codegen': '0.0.0-canary.133',
    '@react-native-windows/find-dotnet-tools': '0.0.0-canary.2',
  });
});

