/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Compares current perf results against stored baselines.
 * Used in CI to detect regressions and generate comparison reports.
 *
 * @format
 */

import type {PerfMetrics} from '../interfaces/PerfMetrics';
import type {PerfThreshold} from '../interfaces/PerfThreshold';
import {DEFAULT_THRESHOLD} from '../interfaces/PerfThreshold';
import type {SnapshotFile} from '../matchers/snapshotManager';
import type {ComparisonResult} from '../reporters/MarkdownReporter';

/**
 * Options for baseline comparison.
 */
export interface CompareOptions {
  /**
   * Default threshold applied when a snapshot entry has none.
   */
  defaultThreshold?: PerfThreshold;

  /**
   * Per-scenario threshold overrides, keyed by snapshot key.
   */
  thresholdOverrides?: Record<string, Partial<PerfThreshold>>;
}

/**
 * Full comparison output for a test suite.
 */
export interface SuiteComparison {
  suiteName: string;
  comparisons: ComparisonResult[];
  hasRegressions: boolean;
}

/**
 * Compares head (current) perf snapshots against base (main) snapshots.
 *
 * Produces ComparisonResult[] that can be fed into MarkdownReporter.
 */
export class BaselineComparator {
  /**
   * Compare a single suite's head snapshot against its baseline.
   *
   * @param suiteName - Human-readable suite name (e.g., 'View')
   * @param headSnapshots - Snapshots from the current (PR) run
   * @param baseSnapshots - Snapshots from the baseline (main) branch
   * @param options - Comparison options
   */
  static compareSuite(
    suiteName: string,
    headSnapshots: SnapshotFile,
    baseSnapshots: SnapshotFile,
    options: CompareOptions = {},
  ): SuiteComparison {
    const defaultThreshold = options.defaultThreshold || DEFAULT_THRESHOLD;
    const comparisons: ComparisonResult[] = [];

    // Compare all entries present in head
    for (const [key, headEntry] of Object.entries(headSnapshots)) {
      const baseEntry = baseSnapshots[key];
      const threshold = {
        ...defaultThreshold,
        ...(headEntry.threshold || {}),
        ...(options.thresholdOverrides?.[key] || {}),
      };

      if (!baseEntry) {
        // New scenario — no baseline to compare
        comparisons.push({
          metrics: headEntry.metrics,
          baselineMetrics: undefined,
          percentChange: undefined,
          passed: true, // New scenarios always pass
        });
        continue;
      }

      const result = BaselineComparator.compareEntry(
        headEntry.metrics,
        baseEntry.metrics,
        threshold,
      );
      comparisons.push(result);
    }

    // Flag scenarios removed in head (existed in base but not in head)
    for (const key of Object.keys(baseSnapshots)) {
      if (!headSnapshots[key]) {
        comparisons.push({
          metrics: {
            ...baseSnapshots[key].metrics,
            name: `[REMOVED] ${baseSnapshots[key].metrics.name}`,
          },
          baselineMetrics: baseSnapshots[key].metrics,
          percentChange: undefined,
          passed: true, // Removal is not a regression
        });
      }
    }

    return {
      suiteName,
      comparisons,
      hasRegressions: comparisons.some(c => !c.passed),
    };
  }

  /**
   * Compare a single entry's metrics against its baseline.
   */
  static compareEntry(
    head: PerfMetrics,
    base: PerfMetrics,
    threshold: PerfThreshold,
  ): ComparisonResult {
    // Merge with defaults so all fields are guaranteed defined
    const resolved: Required<PerfThreshold> = {
      ...DEFAULT_THRESHOLD,
      ...threshold,
    };

    const percentChange =
      base.meanDuration > 0
        ? ((head.meanDuration - base.meanDuration) / base.meanDuration) * 100
        : 0;

    const errors: string[] = [];

    // Check percentage increase
    if (percentChange > resolved.maxDurationIncrease) {
      errors.push(
        `Duration increased by ${percentChange.toFixed(1)}% ` +
          `(threshold: ${resolved.maxDurationIncrease}%)`,
      );
    }

    // Check absolute max
    if (head.meanDuration > resolved.maxDuration) {
      errors.push(
        `Duration ${head.meanDuration.toFixed(2)}ms exceeds ` +
          `max ${resolved.maxDuration}ms`,
      );
    }

    // Check render count
    if (head.renderCount > resolved.maxRenderCount) {
      errors.push(
        `Render count ${head.renderCount} exceeds ` +
          `max ${resolved.maxRenderCount}`,
      );
    }

    return {
      metrics: head,
      baselineMetrics: base,
      percentChange,
      passed: errors.length === 0,
      error: errors.length > 0 ? errors.join('; ') : undefined,
    };
  }
}
