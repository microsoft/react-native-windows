/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {PerfThreshold} from '../interfaces/PerfThreshold';

export interface PerfTestConfig {
  defaultRuns: number;
  defaultWarmupRuns: number;
  defaultThreshold: Required<PerfThreshold>;
  verbose: boolean;
  outputDir: string;
}

export const DEFAULT_CONFIG: Readonly<PerfTestConfig> = {
  defaultRuns: 10,
  defaultWarmupRuns: 1,
  defaultThreshold: {
    maxDurationIncrease: 10,
    maxDuration: Infinity,
    minAbsoluteDelta: 3,
    maxRenderCount: 5,
    minRuns: 10,
  },
  verbose: false,
  outputDir: '.perf-results',
};

export function createPerfConfig(
  overrides: Partial<PerfTestConfig>,
): PerfTestConfig {
  return {
    ...DEFAULT_CONFIG,
    ...overrides,
    defaultThreshold: {
      ...DEFAULT_CONFIG.defaultThreshold,
      ...(overrides.defaultThreshold ?? {}),
    },
  };
}
