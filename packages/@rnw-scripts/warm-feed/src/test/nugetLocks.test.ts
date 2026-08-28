/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {mkdirSync, mkdtempSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {collectNuGetLockTargets, parseNuGetLock} from '../nugetLocks';
import type {Logger} from '../types';

const silentLog: Logger = {
  info() {},
  warn() {},
  error() {},
  debug() {},
};

test('parseNuGetLock extracts resolved versions across frameworks, deduped', () => {
  const lock = {
    version: 1,
    dependencies: {
      'net8.0-windows10.0.19041.0': {
        'Microsoft.Windows.CppWinRT': {type: 'Direct', resolved: '2.0.230706.1'},
        'Some.Transitive': {type: 'Transitive', resolved: '1.2.3'},
        MyProject: {type: 'Project'},
      },
      'native,Version=v0.0': {
        // Same package+version in another TFM must not duplicate.
        'Microsoft.Windows.CppWinRT': {type: 'Direct', resolved: '2.0.230706.1'},
        'Other.Pkg': {type: 'Transitive', resolved: '4.5.6'},
      },
    },
  };
  expect(parseNuGetLock(lock).sort((a, b) => a.id.localeCompare(b.id))).toEqual([
    {id: 'Microsoft.Windows.CppWinRT', version: '2.0.230706.1'},
    {id: 'Other.Pkg', version: '4.5.6'},
    {id: 'Some.Transitive', version: '1.2.3'},
  ]);
});

test('parseNuGetLock skips entries without a resolved version', () => {
  const lock = {
    dependencies: {
      tfm: {
        Central: {type: 'CentralTransitive', requested: '[1.0.0, )'},
        Real: {type: 'Direct', resolved: '1.0.0'},
      },
    },
  };
  expect(parseNuGetLock(lock)).toEqual([{id: 'Real', version: '1.0.0'}]);
});

test('parseNuGetLock tolerates missing/garbage input', () => {
  expect(parseNuGetLock(null)).toEqual([]);
  expect(parseNuGetLock({})).toEqual([]);
  expect(parseNuGetLock({dependencies: null})).toEqual([]);
  expect(parseNuGetLock({dependencies: {tfm: null}})).toEqual([]);
});

test('collectNuGetLockTargets scans a tree, skips node_modules, dedupes to nuget targets', () => {
  const root = mkdtempSync(join(tmpdir(), 'warm-nuget-'));
  const projDir = join(root, 'vnext', 'Desktop');
  mkdirSync(projDir, {recursive: true});
  writeFileSync(
    join(projDir, 'packages.lock.json'),
    JSON.stringify({
      dependencies: {tfm: {'Pkg.A': {type: 'Direct', resolved: '1.0.0'}}},
    }),
  );
  const otherDir = join(root, 'packages', 'app');
  mkdirSync(otherDir, {recursive: true});
  writeFileSync(
    join(otherDir, 'packages.lock.json'),
    JSON.stringify({
      dependencies: {
        tfm: {
          'Pkg.A': {type: 'Direct', resolved: '1.0.0'}, // dup across files
          'Pkg.B': {type: 'Transitive', resolved: '2.0.0'},
        },
      },
    }),
  );
  // A lock file under node_modules must be ignored.
  const nmDir = join(root, 'node_modules', 'x');
  mkdirSync(nmDir, {recursive: true});
  writeFileSync(
    join(nmDir, 'packages.lock.json'),
    JSON.stringify({
      dependencies: {tfm: {'Should.Not.Warm': {resolved: '9.9.9'}}},
    }),
  );

  const targets = collectNuGetLockTargets([root], silentLog).sort((a, b) =>
    a.id.localeCompare(b.id),
  );
  expect(targets).toEqual([
    {ecosystem: 'nuget', id: 'Pkg.A', version: '1.0.0', source: 'nuget-lock'},
    {ecosystem: 'nuget', id: 'Pkg.B', version: '2.0.0', source: 'nuget-lock'},
  ]);
});
