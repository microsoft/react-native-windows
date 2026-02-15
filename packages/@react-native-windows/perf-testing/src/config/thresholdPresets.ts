/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {PerfThreshold} from '../interfaces/PerfThreshold';

/**
 * Pre-configured threshold presets for different component categories.
 */
export const ThresholdPresets: Readonly<
  Record<string, Required<PerfThreshold>>
> = {
  /** Stable core components (View, Text, Image) — tight thresholds */
  core: {
    maxDurationIncrease: 10,
    maxDuration: Infinity,
    minAbsoluteDelta: 3,
    maxRenderCount: 2,
    minRuns: 10,
  },

  /** Complex list/scroll components — slightly relaxed */
  list: {
    maxDurationIncrease: 15,
    maxDuration: Infinity,
    minAbsoluteDelta: 5,
    maxRenderCount: 5,
    minRuns: 5,
  },

  /** Interactive components with animations — more relaxed */
  interactive: {
    maxDurationIncrease: 20,
    maxDuration: Infinity,
    minAbsoluteDelta: 5,
    maxRenderCount: 10,
    minRuns: 10,
  },

  /** Community/custom components — most relaxed */
  community: {
    maxDurationIncrease: 25,
    maxDuration: Infinity,
    minAbsoluteDelta: 5,
    maxRenderCount: 15,
    minRuns: 5,
  },
};
