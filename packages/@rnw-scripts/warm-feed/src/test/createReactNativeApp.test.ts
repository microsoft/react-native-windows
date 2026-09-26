/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {
  appSpecsFromManifest,
  parseCrnAppConfig,
  templateArgs,
} from '../specialModules/createReactNativeApp';
import type {BranchVersions} from '../specialModules/createReactNativeLibrary';

test('parseCrnAppConfig requires a non-empty branches list', () => {
  expect(() => parseCrnAppConfig({})).toThrow(/branches/);
  expect(() => parseCrnAppConfig({branches: []})).toThrow(/branches/);
});

test('parseCrnAppConfig applies defaults and parses branches', () => {
  const cfg = parseCrnAppConfig({
    branches: [{name: 'main', nightly: true}, {name: '0.85-stable'}],
  });
  expect(cfg.nightlyTemplate).toBe('@react-native-community/template@0.86.0');
  expect(cfg.appName).toBe('warmapp');
  expect(cfg.branches).toHaveLength(2);
  expect(cfg.branches[0]).toMatchObject({name: 'main', nightly: true});
  expect(cfg.branches[1]).toMatchObject({name: '0.85-stable', nightly: false});
});

test('parseCrnAppConfig accepts custom nightlyTemplate and appName', () => {
  const cfg = parseCrnAppConfig({
    nightlyTemplate: '@react-native-community/template@0.87.0',
    appName: 'probe',
    branches: [{name: 'main', nightly: true}],
  });
  expect(cfg.nightlyTemplate).toBe('@react-native-community/template@0.87.0');
  expect(cfg.appName).toBe('probe');
});

test('parseCrnAppConfig throws when a branch has no name', () => {
  expect(() => parseCrnAppConfig({branches: [{}]})).toThrow(/name/);
});

test('parseCrnAppConfig carries per-branch overrides', () => {
  const cfg = parseCrnAppConfig({
    branches: [
      {
        name: '0.83-stable',
        reactNativeVersion: '0.83.7',
        reactNativeCliVersion: '19.0.0',
        reactNativeWindowsSpec: '0.83.0-canary',
        ref: 'refs/heads/0.83-stable',
      },
    ],
  });
  expect(cfg.branches[0]).toMatchObject({
    name: '0.83-stable',
    reactNativeVersion: '0.83.7',
    reactNativeCliVersion: '19.0.0',
    reactNativeWindowsSpec: '0.83.0-canary',
    ref: 'refs/heads/0.83-stable',
  });
});

test('templateArgs pins the stable template for a nightly, none for a stable RN', () => {
  const cfg = parseCrnAppConfig({branches: [{name: 'main', nightly: true}]});
  expect(templateArgs(cfg, true)).toEqual([
    '--template',
    '@react-native-community/template@0.86.0',
  ]);
  expect(templateArgs(cfg, false)).toEqual([]);
});

test('appSpecsFromManifest keeps stable deps and adds the resolved RNW spec', () => {
  const versions: BranchVersions = {
    reactNative: '0.85.3',
    reactNativeCli: '20.0.0',
    reactNativeWindowsSpec: '0.85.3',
    nightly: false,
  };
  const sets = appSpecsFromManifest(
    {
      dependencies: {
        'react-native': '0.85.3',
        'react-native-safe-area-context': '5.10.0',
        react: '19.1.0',
      },
    },
    versions,
    '0.85-stable',
  );
  expect(sets).toHaveLength(1);
  expect(sets[0].label).toBe('crnapp:0.85-stable:app');
  expect(sets[0].specs).toEqual({
    'react-native': '0.85.3',
    'react-native-safe-area-context': '5.10.0',
    react: '19.1.0',
    'react-native-windows': '0.85.3',
  });
});

test('appSpecsFromManifest rewrites the RN family for a nightly branch', () => {
  const versions: BranchVersions = {
    reactNative: '0.86.0-nightly-x',
    reactNativeCli: '21.0.0',
    reactNativeWindowsSpec: undefined,
    nightly: true,
  };
  const [set] = appSpecsFromManifest(
    {
      dependencies: {
        'react-native': '0.86.0',
        '@react-native/new-app-screen': '0.86.0',
        '@react-native-community/cli': '20.0.0',
        'react-native-safe-area-context': '5.10.0',
        react: '19.1.0',
      },
    },
    versions,
    'main',
  );
  expect(set.specs['react-native']).toBe('0.86.0-nightly-x');
  expect(set.specs['@react-native/new-app-screen']).toBe('0.86.0-nightly-x');
  expect(set.specs['@react-native-community/cli']).toBe('21.0.0');
  // Third-party and react are untouched; no RNW spec added when it is undefined.
  expect(set.specs['react-native-safe-area-context']).toBe('5.10.0');
  expect(set.specs.react).toBe('19.1.0');
  expect(set.specs['react-native-windows']).toBeUndefined();
});
