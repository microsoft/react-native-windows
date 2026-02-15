/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {PerfThreshold} from '../interfaces/PerfThreshold';

/**
 * Pre-configured threshold presets for different component categories.
 *
 * Use these as starting points when configuring component tests.
 * Override individual fields as needed.
 *
 * @example
 * ```typescript
 * import { ThresholdPresets } from '@react-native-windows/perf-testing';
 *
 * class MyListPerfTest extends ComponentPerfTestBase {
 *   get defaultThreshold() {
 *     return ThresholdPresets.list;
 *   }
 * }
 * ```
 */
export const ThresholdPresets: Readonly<
  Record<string, Required<PerfThreshold>>
> = {
  /**
   * For stable core components (View, Text, Image).
   * Tight thresholds — these should not regress.
   */
  core: {
    maxDurationIncrease: 10,
    maxDuration: Infinity,
    minAbsoluteDelta: 3,
    maxRenderCount: 2,
    minRuns: 10,
  },

  /**
   * For complex list/scroll components (FlatList, SectionList, ScrollView).
   * Slightly relaxed — these have more variability.
   */
  list: {
    maxDurationIncrease: 15,
    maxDuration: Infinity,
    minAbsoluteDelta: 5,
    maxRenderCount: 5,
    minRuns: 5,
  },

  /**
   * For interactive components with animations (RefreshControl, Touchable).
   * More relaxed to account for animation overhead.
   */
  interactive: {
    maxDurationIncrease: 20,
    maxDuration: Infinity,
    minAbsoluteDelta: 5,
    maxRenderCount: 10,
    minRuns: 10,
  },

  /**
   * For community/custom native components.
   * Most relaxed — gives new components room to stabilize.
   */
  community: {
    maxDurationIncrease: 25,
    maxDuration: Infinity,
    minAbsoluteDelta: 5,
    maxRenderCount: 15,
    minRuns: 5,
  },
};
