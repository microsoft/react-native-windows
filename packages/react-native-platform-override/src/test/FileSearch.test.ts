/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from 'node:fs';
import path from 'path';
import {
  enumerateRepoPackages,
  WritableNpmPackage,
} from '@react-native-windows/package-utils';

import {findAllManifests} from '../FileSearch';

jest.mock('@react-native-windows/package-utils', () => ({
  ...jest.requireActual('@react-native-windows/package-utils'),
  enumerateRepoPackages: jest.fn(),
}));

const root = path.resolve('manifest-discovery-fixture');
const rootManifest = path.join(root, 'overrides.json');
const workspaceDir = path.join(root, 'packages', 'sample');
const workspaceManifest = path.join(workspaceDir, 'overrides.json');

beforeEach(() => {
  jest.spyOn(process, 'cwd').mockReturnValue(root);
  jest
    .spyOn(fs, 'existsSync')
    .mockImplementation(
      file =>
        file === path.join(root, 'package.json') ||
        file === rootManifest ||
        file === workspaceManifest,
    );
  jest.spyOn(fs, 'readFileSync').mockReturnValue(
    JSON.stringify({
      private: true,
      workspaces: {packages: ['packages/*']},
    }),
  );
  jest.mocked(enumerateRepoPackages).mockImplementation(async pred => {
    const pkg = new WritableNpmPackage(workspaceDir, {name: 'sample'});
    return !pred || (await pred(pkg)) ? [pkg] : [];
  });
});

afterEach(() => {
  jest.restoreAllMocks();
  jest.clearAllMocks();
});

test('includes root and workspace override manifests at a workspace root', async () => {
  expect(await findAllManifests()).toEqual([rootManifest, workspaceManifest]);
});

test('supports the array form of workspaces', async () => {
  jest
    .mocked(fs.readFileSync)
    .mockReturnValue(JSON.stringify({workspaces: ['packages/*']}));
  expect(await findAllManifests()).toEqual([rootManifest, workspaceManifest]);
});

test('continues discovering workspaces when there is no root manifest', async () => {
  jest
    .mocked(fs.existsSync)
    .mockImplementation(
      file =>
        file === path.join(root, 'package.json') || file === workspaceManifest,
    );
  expect(await findAllManifests()).toEqual([workspaceManifest]);
});

test('prefers the local manifest outside a workspace root', async () => {
  jest
    .mocked(fs.readFileSync)
    .mockReturnValue(JSON.stringify({name: 'sample'}));
  expect(await findAllManifests()).toEqual([rootManifest]);
  expect(enumerateRepoPackages).not.toHaveBeenCalled();
});
