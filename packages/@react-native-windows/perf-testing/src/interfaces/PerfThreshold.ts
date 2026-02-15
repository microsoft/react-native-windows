/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Thresholds for pass/fail determination of a performance test.
 *
 * When comparing against a baseline snapshot, these thresholds
 * determine whether a regression has occurred.
 */
export interface PerfThreshold {
  /** Max allowed duration increase from baseline (percentage). Default: 10 */
  maxDurationIncrease?: number;

  /** Absolute max duration (ms) — fails if exceeded regardless of baseline */
  maxDuration?: number;

  /**
   * Minimum absolute duration change (ms) required to flag a regression.
   * Even if the percentage threshold is exceeded, regressions below this
   * absolute delta are treated as environmental noise. Default: 3
   */
  minAbsoluteDelta?: number;

  /** Max allowed render count per measurement */
  maxRenderCount?: number;

  /** Minimum runs required for a valid measurement. Default: 10 */
  minRuns?: number;
}

/**
 * Default thresholds applied to all component tests unless overridden.
 *
 * - 10% regression threshold balances noise filtering vs real detection.
 * - 5 max render count is generous for most single-component scenarios.
 * - 10 runs provides statistical significance without being too slow.
 */
export const DEFAULT_THRESHOLD: Readonly<Required<PerfThreshold>> = {
  maxDurationIncrease: 10,
  maxDuration: Infinity,
  minAbsoluteDelta: 3,
  maxRenderCount: 5,
  minRuns: 10,
};
