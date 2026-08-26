/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {isResolvableSpec, manifestSpecs, sanitizeSpecs} from '../manifest';

test('isResolvableSpec keeps registry ranges/tags/aliases', () => {
  for (const s of [
    '^1.0.0',
    '~2.3.4',
    '1.2.3',
    '*',
    'latest',
    'next',
    '>=1 <2',
    'npm:foo@1.2.3',
  ]) {
    expect(isResolvableSpec(s)).toBe(true);
  }
});

test('isResolvableSpec drops local, VCS, URL, and GitHub-shorthand specs', () => {
  for (const s of [
    'workspace:*',
    'workspace:^',
    'link:../x',
    'file:../x',
    'portal:../x',
    'patch:foo@1.0.0',
    'git+https://x/y.git',
    'git+ssh://git@x/y.git',
    'https://x/y.tgz',
    'user/repo',
    'user/repo#semver:^1.0.0',
    '',
  ]) {
    expect(isResolvableSpec(s)).toBe(false);
  }
});

test('sanitizeSpecs filters non-registry entries', () => {
  expect(
    sanitizeSpecs({a: '^1.0.0', b: 'file:../b', c: 'workspace:*', d: 'latest'}),
  ).toEqual({a: '^1.0.0', d: 'latest'});
});

test('manifestSpecs merges sections; a concrete dep/devDep beats a peer range', () => {
  const pkg = {
    dependencies: {a: '^1.0.0', local: 'file:../l'},
    devDependencies: {b: '~2.0.0', 'react-native': '0.85.0'},
    optionalDependencies: {c: '^3.0.0'},
    peerDependencies: {'react-native': '*'},
  };
  expect(manifestSpecs(pkg)).toEqual({
    a: '^1.0.0',
    b: '~2.0.0',
    c: '^3.0.0',
    'react-native': '0.85.0',
  });
});

test('manifestSpecs tolerates a manifest with no dependency sections', () => {
  expect(manifestSpecs({name: 'x', version: '1.0.0'})).toEqual({});
  expect(manifestSpecs(null)).toEqual({});
});
