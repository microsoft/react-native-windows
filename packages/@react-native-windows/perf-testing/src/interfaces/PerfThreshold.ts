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

  /**
   * Max coefficient of variation (stdDev/mean) allowed for reliable comparison.
   * Tests above this CV are too noisy to gate on — they warn instead of fail.
   * Default: 0.5 (50%). Set lower for stricter noise filtering.
   */
  maxCV?: number;

  /**
   * 'gate' = fail CI on regression (default for stable tests).
   * 'track' = warn only, never fail (for inherently noisy bulk scenarios).
   */
  mode?: 'gate' | 'track';
}

export const DEFAULT_THRESHOLD: Readonly<Required<PerfThreshold>> = {
  maxDurationIncrease: 10,
  maxDuration: Infinity,
  minAbsoluteDelta: 3,
  maxRenderCount: 5,
  minRuns: 10,
  maxCV: 0.5,
  mode: 'gate',
};
