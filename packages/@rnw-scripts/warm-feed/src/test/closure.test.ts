/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {parseNpmLock} from '../closure';

test('parses a v3 packages map, skipping root/links/non-registry entries', () => {
  const lock = {
    packages: {
      '': {name: 'root', version: '0.0.0'},
      'node_modules/left-pad': {
        version: '1.3.0',
        resolved: 'https://feed/left-pad-1.3.0.tgz',
      },
      'node_modules/@scope/pkg': {
        version: '2.0.0',
        resolved: 'https://feed/pkg-2.0.0.tgz',
      },
      'node_modules/a/node_modules/b': {
        version: '0.1.0',
        resolved: 'https://feed/b.tgz',
      },
      'node_modules/linked': {version: '1.0.0', link: true},
      'node_modules/local': {version: '1.0.0', resolved: 'file:../local'},
      'node_modules/fromgit': {
        version: '1.0.0',
        resolved: 'git+https://x/y.git',
      },
      'node_modules/noversion': {resolved: 'https://feed/x'},
    },
  };
  const got = parseNpmLock(lock)
    .map(e => `${e.id}@${e.version}`)
    .sort();
  expect(got).toEqual(['@scope/pkg@2.0.0', 'b@0.1.0', 'left-pad@1.3.0']);
});

test('dedupes a name@version that appears at multiple depths', () => {
  const lock = {
    packages: {
      'node_modules/x': {version: '1.0.0', resolved: 'https://f/x'},
      'node_modules/y/node_modules/x': {
        version: '1.0.0',
        resolved: 'https://f/x',
      },
    },
  };
  expect(parseNpmLock(lock)).toEqual([{id: 'x', version: '1.0.0'}]);
});

test('falls back to the v1 dependencies tree', () => {
  const lock = {
    dependencies: {
      foo: {
        version: '1.0.0',
        resolved: 'https://f/foo',
        dependencies: {
          bar: {version: '2.0.0', resolved: 'https://f/bar'},
        },
      },
    },
  };
  const got = parseNpmLock(lock)
    .map(e => `${e.id}@${e.version}`)
    .sort();
  expect(got).toEqual(['bar@2.0.0', 'foo@1.0.0']);
});

test('an empty or garbage lock yields nothing', () => {
  expect(parseNpmLock({})).toEqual([]);
  expect(parseNpmLock(null)).toEqual([]);
  expect(parseNpmLock({packages: {}})).toEqual([]);
});
