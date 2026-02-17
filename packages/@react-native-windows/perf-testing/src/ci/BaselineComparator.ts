/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
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
  defaultThreshold?: PerfThreshold;
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
 */
export class BaselineComparator {
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
        ...headEntry.threshold,
        ...(options.thresholdOverrides?.[key] || {}),
      };

      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (!baseEntry) {
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

    // Flag scenarios removed in head
    for (const key of Object.keys(baseSnapshots)) {
      // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
      if (!headSnapshots[key]) {
        comparisons.push({
          metrics: {
            ...baseSnapshots[key].metrics,
            name: `[REMOVED] ${baseSnapshots[key].metrics.name}`,
          },
          baselineMetrics: baseSnapshots[key].metrics,
          percentChange: undefined,
          passed: true,
        });
      }
    }

    return {
      suiteName,
      comparisons,
      hasRegressions: comparisons.some(c => !c.passed),
    };
  }

  static compareEntry(
    head: PerfMetrics,
    base: PerfMetrics,
    threshold: PerfThreshold,
  ): ComparisonResult {
    const resolved: Required<PerfThreshold> = {
      ...DEFAULT_THRESHOLD,
      ...threshold,
    };

    const percentChange =
      base.medianDuration > 0
        ? ((head.medianDuration - base.medianDuration) / base.medianDuration) *
          100
        : head.medianDuration > 0
        ? Infinity
        : 0;

    const errors: string[] = [];

    // Check percentage increase AND absolute delta
    const absoluteDelta = head.medianDuration - base.medianDuration;
    if (
      percentChange > resolved.maxDurationIncrease &&
      absoluteDelta > resolved.minAbsoluteDelta
    ) {
      errors.push(
        `Duration increased by ${percentChange.toFixed(
          1,
        )}% / +${absoluteDelta.toFixed(2)}ms ` +
          `(threshold: ${resolved.maxDurationIncrease}% & ${resolved.minAbsoluteDelta}ms)`,
      );
    }

    // Check absolute max
    if (head.medianDuration > resolved.maxDuration) {
      errors.push(
        `Duration ${head.medianDuration.toFixed(2)}ms exceeds ` +
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
