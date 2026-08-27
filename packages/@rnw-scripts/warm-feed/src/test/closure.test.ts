/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {mkdtempSync, readFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {parseNpmLock, writeFeedNpmrc} from '../closure';
import type {Auth} from '../types';

const fakeAuth: Auth = {
  kind: 'pat',
  header: async () => ({}),
  token: async () => 'SECRET-TOKEN',
};

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

test('prefers the lock entry name over the alias in the path key', () => {
  const lock = {
    packages: {
      'node_modules/string-width-cjs': {
        name: 'string-width',
        version: '4.2.3',
        resolved: 'https://feed/string-width-4.2.3.tgz',
      },
    },
  };
  expect(parseNpmLock(lock)).toEqual([{id: 'string-width', version: '4.2.3'}]);
});

test('an empty or garbage lock yields nothing', () => {
  expect(parseNpmLock({})).toEqual([]);
  expect(parseNpmLock(null)).toEqual([]);
  expect(parseNpmLock({packages: {}})).toEqual([]);
});

test('writeFeedNpmrc attaches the token only to an Azure DevOps feed host', async () => {
  const adoDir = mkdtempSync(join(tmpdir(), 'warm-npmrc-'));
  const adoPath = await writeFeedNpmrc(
    fakeAuth,
    'https://pkgs.dev.azure.com/org/proj/_packaging/feed/npm/registry/',
    adoDir,
  );
  expect(readFileSync(adoPath, 'utf8')).toContain(':_authToken=SECRET-TOKEN');

  const foreignDir = mkdtempSync(join(tmpdir(), 'warm-npmrc-'));
  const foreignPath = await writeFeedNpmrc(
    fakeAuth,
    'https://registry.npmjs.org/',
    foreignDir,
  );
  const foreign = readFileSync(foreignPath, 'utf8');
  expect(foreign).toContain('registry=https://registry.npmjs.org/');
  expect(foreign).not.toContain('_authToken');
  expect(foreign).not.toContain('SECRET-TOKEN');
});
