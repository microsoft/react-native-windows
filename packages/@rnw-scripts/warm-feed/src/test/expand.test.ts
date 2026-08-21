/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {expandPackage} from '../expand';
import type {NpmRegistry, NuGetRegistry} from '../registries';
import type {ExpandConfig, FeedPackage} from '../types';

function npmRegistry(pool: string[]): NpmRegistry {
  return {
    getVersions: async () => pool,
    getTarballUrl: async () => null,
  };
}

function nugetRegistry(pool: string[]): NuGetRegistry {
  return {
    getVersions: async () => pool,
    nupkgUrl: async () => '',
  };
}

const cfg = (over: Partial<ExpandConfig> = {}): ExpandConfig => ({
  scope: 'in-use-lines',
  includeLatest: false,
  includePrerelease: false,
  maxMajorsBack: 0,
  ...over,
});

const versionsOf = (targets: ReadonlyArray<{version: string}>) =>
  targets.map(t => t.version).sort();

const pkg: FeedPackage = {
  ecosystem: 'npm',
  id: 'demo',
  savedVersions: ['1.0.0', '1.1.0', '2.0.0'],
};
const pool = [
  '1.0.0',
  '1.0.1',
  '1.1.0',
  '1.1.5',
  '2.0.0',
  '2.1.0',
  '3.0.0-beta.1',
];

test('in-use-lines warms the latest patch of each saved major.minor', async () => {
  const t = await expandPackage('npm', pkg, npmRegistry(pool), cfg());
  expect(versionsOf(t)).toEqual(['1.0.1', '1.1.5', '2.0.0']);
});

test('in-use-majors collapses to the latest of each saved major', async () => {
  const t = await expandPackage(
    'npm',
    pkg,
    npmRegistry(pool),
    cfg({scope: 'in-use-majors'}),
  );
  expect(versionsOf(t)).toEqual(['1.1.5', '2.1.0']);
});

test('includeLatest adds the newest stable version overall', async () => {
  const t = await expandPackage(
    'npm',
    pkg,
    npmRegistry(pool),
    cfg({includeLatest: true}),
  );
  expect(versionsOf(t)).toEqual(['1.0.1', '1.1.5', '2.0.0', '2.1.0']);
});

test('maxMajorsBack keeps only the most recent saved majors', async () => {
  const t = await expandPackage(
    'npm',
    pkg,
    npmRegistry(pool),
    cfg({scope: 'in-use-majors', maxMajorsBack: 1}),
  );
  expect(versionsOf(t)).toEqual(['2.1.0']);
});

test('prereleases are skipped unless includePrerelease is set', async () => {
  const preOnly: FeedPackage = {...pkg, savedVersions: ['3.0.0-beta.1']};
  expect(
    await expandPackage(
      'npm',
      preOnly,
      npmRegistry(pool),
      cfg({includeLatest: true}),
    ),
  ).toEqual([]);
  const withPre = await expandPackage(
    'npm',
    preOnly,
    npmRegistry(pool),
    cfg({includePrerelease: true, includeLatest: true}),
  );
  expect(versionsOf(withPre)).toContain('3.0.0-beta.1');
});

test('a package with no versions in the registry expands to nothing', async () => {
  expect(await expandPackage('npm', pkg, npmRegistry([]), cfg())).toEqual([]);
});

test('nuget expansion warms the latest patch of each saved line', async () => {
  const npkg: FeedPackage = {
    ecosystem: 'nuget',
    id: 'Demo',
    savedVersions: ['1.0.0', '1.1.0'],
  };
  const npool = ['1.0.0', '1.0.5', '1.1.0', '1.1.2', '2.0.0'];
  const t = await expandPackage('nuget', npkg, nugetRegistry(npool), cfg());
  expect(versionsOf(t)).toEqual(['1.0.5', '1.1.2']);
});
