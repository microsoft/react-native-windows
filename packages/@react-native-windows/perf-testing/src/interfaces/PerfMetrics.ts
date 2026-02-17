/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Performance metrics collected from a single test scenario run.
 *
 * All duration values are in milliseconds.
 */
export interface PerfMetrics {
  /** Test scenario name (e.g., 'View mount') */
  name: string;

  /** Mean duration across all runs (ms) */
  meanDuration: number;

  /** Median duration across all runs (ms) */
  medianDuration: number;

  /** Standard deviation of durations (ms) */
  stdDev: number;

  /** Average render count per run */
  renderCount: number;

  /** Number of test runs performed */
  runs: number;

  /** Raw durations from each run (ms), used for statistical comparison */
  durations: number[];

  /** ISO timestamp of when the measurement was taken */
  timestamp: string;

  /**
   * Extension point for native timing data.
   * Reserved for future integration with native perf markers.
   */
  nativeTimings?: Record<string, number>;

  /**
   * Extension point for memory measurements.
   * Reserved for future memory tracking integration.
   */
  memoryDelta?: number;
}
