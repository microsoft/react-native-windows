/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {PerfMetrics} from '../interfaces/PerfMetrics';
import type {PerfThreshold} from '../interfaces/PerfThreshold';
import {DEFAULT_THRESHOLD} from '../interfaces/PerfThreshold';
import {SnapshotManager} from './snapshotManager';

/**
 * Extend Jest's Matchers interface so `toMatchPerfSnapshot` is recognized.
 */
declare global {
  // eslint-disable-next-line @typescript-eslint/no-namespace
  namespace jest {
    interface Matchers<R> {
      toMatchPerfSnapshot(threshold?: Partial<PerfThreshold>): R;
    }
  }
}

expect.extend({
  toMatchPerfSnapshot(
    received: PerfMetrics,
    customThreshold?: Partial<PerfThreshold>,
  ) {
    const testPath = expect.getState().testPath!;
    const testName = expect.getState().currentTestName!;

    // Resolve snapshot file location
    const {file: snapshotFile} =
      SnapshotManager.getSnapshotPath(testPath);
    const snapshotKey = SnapshotManager.buildKey(testName);

    const isUpdateMode =
      process.argv.includes('-u') ||
      process.argv.includes('--updateSnapshot');

    // Load existing snapshots
    const snapshots = SnapshotManager.load(snapshotFile);
    const baseline = snapshots[snapshotKey];

    const threshold: PerfThreshold = {...DEFAULT_THRESHOLD, ...customThreshold};

    // UPDATE MODE or FIRST RUN: write new baseline
    if (isUpdateMode || !baseline) {
      snapshots[snapshotKey] = {
        metrics: received,
        threshold,
        capturedAt: new Date().toISOString(),
      };

      SnapshotManager.save(snapshotFile, snapshots);

      return {
        pass: true,
        message: () =>
          `✅ Perf snapshot ${baseline ? 'updated' : 'created'}: ${received.name}` +
          ` (${received.meanDuration.toFixed(2)}ms, ${received.renderCount} renders)`,
      };
    }

    // COMPARE MODE: check against baseline
    const errors: string[] = [];

    // Check duration regression using MEDIAN
    if (threshold.maxDurationIncrease !== undefined) {
      const percentChange =
        ((received.medianDuration - baseline.metrics.medianDuration) /
          baseline.metrics.medianDuration) *
        100;

      const absoluteDelta = received.medianDuration - baseline.metrics.medianDuration;
      const minAbsoluteDelta = threshold.minAbsoluteDelta ?? DEFAULT_THRESHOLD.minAbsoluteDelta;

      if (percentChange > threshold.maxDurationIncrease && absoluteDelta > minAbsoluteDelta) {
        errors.push(
          `Duration regression: +${percentChange.toFixed(1)}% / +${absoluteDelta.toFixed(2)}ms ` +
            `(baseline median: ${baseline.metrics.medianDuration.toFixed(2)}ms → ` +
            `current median: ${received.medianDuration.toFixed(2)}ms, ` +
            `threshold: ${threshold.maxDurationIncrease}% & ${minAbsoluteDelta}ms)`,
        );
      }
    }

    // Check absolute duration limit
    if (
      threshold.maxDuration !== undefined &&
      threshold.maxDuration !== Infinity &&
      received.medianDuration > threshold.maxDuration
    ) {
      errors.push(
        `Duration exceeded: ${received.medianDuration.toFixed(2)}ms > ` +
          `${threshold.maxDuration}ms (absolute limit)`,
      );
    }

    // Check render count
    if (
      threshold.maxRenderCount !== undefined &&
      received.renderCount > threshold.maxRenderCount
    ) {
      errors.push(
        `Render count exceeded: ${received.renderCount} > ` +
          `${threshold.maxRenderCount}`,
      );
    }

    if (errors.length > 0) {
      return {
        pass: false,
        message: () =>
          `❌ Performance regression detected in "${received.name}":\n\n` +
          errors.map(e => `  • ${e}`).join('\n') +
          `\n\n📊 Baseline captured: ${baseline.capturedAt}` +
          `\n💡 Run with -u flag to update baseline if intentional.`,
      };
    }

    // Calculate improvement for positive feedback
    const improvement =
      ((baseline.metrics.medianDuration - received.medianDuration) /
        baseline.metrics.medianDuration) *
      100;

    return {
      pass: true,
      message: () =>
        `✅ "${received.name}" within threshold ` +
        `(median: ${received.medianDuration.toFixed(2)}ms, ` +
        `${improvement > 0 ? `-${improvement.toFixed(1)}% faster` : 'no change'})`,
    };
  },
});
