/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {readFileSync} from 'node:fs';
import type {ExpandConfig, WarmerConfig} from './types';

const DEFAULT_EXPAND: ExpandConfig = {
  scope: 'in-use-lines',
  includeLatest: true,
  includePrerelease: false,
  maxMajorsBack: 0,
};

export function loadConfig(configPath: string): WarmerConfig {
  let raw: Partial<WarmerConfig>;
  try {
    raw = JSON.parse(readFileSync(configPath, 'utf8')) as Partial<WarmerConfig>;
  } catch (err) {
    throw new Error(
      `failed to read config '${configPath}': ${(err as Error).message}`,
    );
  }
  if (!raw.feeds || (!raw.feeds.npm && !raw.feeds.nuget)) {
    throw new Error(
      `config '${configPath}' must define at least one of feeds.npm / feeds.nuget`,
    );
  }
  if (!raw.enumerate?.feedManagementBase) {
    throw new Error(
      `config '${configPath}' must define enumerate.feedManagementBase`,
    );
  }
  return {
    feeds: raw.feeds,
    enumerate: {
      feedManagementBase: raw.enumerate.feedManagementBase,
      apiVersion: raw.enumerate.apiVersion ?? '7.1',
      pageSize: raw.enumerate.pageSize ?? 1000,
    },
    expand: {...DEFAULT_EXPAND, ...(raw.expand ?? {})},
    concurrency: raw.concurrency ?? 8,
    ignore: raw.ignore ?? [],
  };
}
