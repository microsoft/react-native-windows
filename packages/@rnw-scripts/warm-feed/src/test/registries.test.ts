/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

jest.mock('../http');

import * as http from '../http';
import {createNpmRegistry, createNuGetRegistry} from '../registries';
import type {Auth, Logger} from '../types';

const mockFetchJson = http.fetchJson as jest.Mock;

const auth: Auth = {kind: 'pat', header: async () => ({})};
const log: Logger = {
  info: () => {},
  warn: () => {},
  error: () => {},
  debug: () => {},
};

beforeEach(() => mockFetchJson.mockReset());

describe('npm registry', () => {
  const reg = () => createNpmRegistry('https://feed/npm/', auth, log);

  test('returns the versions of a valid packument', async () => {
    mockFetchJson.mockResolvedValue({
      status: 200,
      body: {versions: {'1.0.0': {}, '1.1.0': {}}},
    });
    expect(await reg().getVersions('left-pad')).toEqual(['1.0.0', '1.1.0']);
  });

  test('treats a 404 as not-in-feed (empty list, not an error)', async () => {
    mockFetchJson.mockResolvedValue({status: 404, body: null});
    expect(await reg().getVersions('missing')).toEqual([]);
  });

  test('throws on a 5xx feed error instead of reporting no versions', async () => {
    mockFetchJson.mockResolvedValue({status: 503, body: null});
    await expect(reg().getVersions('left-pad')).rejects.toThrow(/503/);
  });

  test('throws on an auth failure', async () => {
    mockFetchJson.mockResolvedValue({status: 403, body: null});
    await expect(reg().getVersions('left-pad')).rejects.toThrow(/auth failed/);
  });

  test('throws on a malformed 200 body', async () => {
    mockFetchJson.mockResolvedValue({status: 200, body: null});
    await expect(reg().getVersions('left-pad')).rejects.toThrow(/fetch failed/);
  });
});

describe('nuget registry', () => {
  const INDEX = 'https://feed/nuget/v3/index.json';
  const serviceIndex = {
    status: 200,
    body: {
      resources: [
        {
          '@type': 'PackageBaseAddress/3.0.0',
          '@id': 'https://feed/nuget/flat2/',
        },
      ],
    },
  };
  const mockNuget = (versionResp: {status: number; body: unknown}) =>
    mockFetchJson.mockImplementation(async (url: string) =>
      url === INDEX ? serviceIndex : versionResp,
    );

  test('returns the versions of a valid index', async () => {
    mockNuget({status: 200, body: {versions: ['1.0.0', '1.1.0']}});
    expect(
      await createNuGetRegistry(INDEX, auth, log).getVersions('Foo'),
    ).toEqual(['1.0.0', '1.1.0']);
  });

  test('treats a 404 as not-in-feed (empty list)', async () => {
    mockNuget({status: 404, body: null});
    expect(
      await createNuGetRegistry(INDEX, auth, log).getVersions('Foo'),
    ).toEqual([]);
  });

  test('throws on a 5xx feed error instead of reporting no versions', async () => {
    mockNuget({status: 500, body: null});
    await expect(
      createNuGetRegistry(INDEX, auth, log).getVersions('Foo'),
    ).rejects.toThrow(/500/);
  });
});
