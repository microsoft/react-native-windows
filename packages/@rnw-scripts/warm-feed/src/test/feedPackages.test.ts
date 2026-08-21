/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

jest.mock('../http');

import * as http from '../http';
import {enumerateFeed} from '../feedPackages';
import type {Auth, Logger, WarmerConfig} from '../types';

const mockFetchJson = http.fetchJson as jest.Mock;

const auth: Auth = {kind: 'pat', header: async () => ({})};
const log: Logger = {
  info: () => {},
  warn: () => {},
  error: () => {},
  debug: () => {},
};

const config: WarmerConfig = {
  feeds: {},
  enumerate: {
    feedManagementBase: 'https://feeds/base',
    apiVersion: '7.1',
    pageSize: 50,
  },
  expand: {
    scope: 'in-use-lines',
    includeLatest: false,
    includePrerelease: false,
    maxMajorsBack: 0,
  },
  concurrency: 1,
  ignore: [],
};

beforeEach(() => mockFetchJson.mockReset());

test('returns feed packages and their non-deleted saved versions', async () => {
  mockFetchJson.mockResolvedValue({
    status: 200,
    body: {
      value: [
        {name: 'a', versions: [{version: '1.0.0'}, {version: '1.1.0'}]},
        {name: 'b', versions: [{version: '2.0.0', isDeleted: true}]},
        {versions: [{version: '9.9.9'}]}, // no name -> skipped
      ],
    },
  });
  const pkgs = await enumerateFeed(config, 'npm', auth, log);
  expect(pkgs).toEqual([
    {ecosystem: 'npm', id: 'a', savedVersions: ['1.0.0', '1.1.0']},
    {ecosystem: 'npm', id: 'b', savedVersions: []},
  ]);
});

test('follows pagination until a short page ends enumeration', async () => {
  const paged: WarmerConfig = {
    ...config,
    enumerate: {...config.enumerate, pageSize: 2},
  };
  mockFetchJson.mockImplementation(async (url: string) => {
    if (url.includes('$skip=0')) {
      return {
        status: 200,
        body: {
          value: [
            {name: 'a', versions: []},
            {name: 'b', versions: []},
          ],
        },
      };
    }
    return {status: 200, body: {value: [{name: 'c', versions: []}]}};
  });
  const pkgs = await enumerateFeed(paged, 'nuget', auth, log);
  expect(pkgs.map(p => p.id)).toEqual(['a', 'b', 'c']);
  expect(mockFetchJson).toHaveBeenCalledTimes(2);
});

test('throws on an auth failure', async () => {
  mockFetchJson.mockResolvedValue({status: 401, body: null});
  await expect(enumerateFeed(config, 'npm', auth, log)).rejects.toThrow(
    /auth failed/,
  );
});

test('throws on a 5xx error instead of ending enumeration empty', async () => {
  mockFetchJson.mockResolvedValue({status: 500, body: null});
  await expect(enumerateFeed(config, 'npm', auth, log)).rejects.toThrow(/500/);
});

test('throws on a malformed (null) body', async () => {
  mockFetchJson.mockResolvedValue({status: 200, body: null});
  await expect(enumerateFeed(config, 'npm', auth, log)).rejects.toThrow(
    /Get Packages failed/,
  );
});

test('throws on a wrong-shape page (no value array)', async () => {
  mockFetchJson.mockResolvedValue({status: 200, body: {}});
  await expect(enumerateFeed(config, 'npm', auth, log)).rejects.toThrow(
    /Get Packages failed/,
  );
});
