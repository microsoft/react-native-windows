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
import type {SnapshotEntry} from './snapshotManager';
import {coefficientOfVariation, mannWhitneyU} from '../core/statistics';

interface CompareResult {
  errors: string[];
  warnings: string[];
}

function checkNoiseGates(
  received: PerfMetrics,
  baseline: SnapshotEntry,
  threshold: PerfThreshold,
): {tooNoisy: boolean; statSignificant: boolean; warnings: string[]} {
  const warnings: string[] = [];

  // CV gate
  const cv =
    // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
    received.durations && received.durations.length >= 2
      ? coefficientOfVariation(received.durations)
      : 0;
  const maxCV = threshold.maxCV ?? DEFAULT_THRESHOLD.maxCV;
  const tooNoisy = cv > maxCV;

  if (tooNoisy) {
    warnings.push(
      `High variance (CV=${(cv * 100).toFixed(1)}% > ${(maxCV * 100).toFixed(
        0,
      )}%) — skipping regression check`,
    );
  }

  // Mann-Whitney U statistical significance
  let statSignificant = true;
  if (
    !tooNoisy &&
    // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
    received.durations &&
    received.durations.length >= 2 &&
    // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
    baseline.metrics.durations &&
    baseline.metrics.durations.length >= 2
  ) {
    const mwResult = mannWhitneyU(
      baseline.metrics.durations,
      received.durations,
    );
    statSignificant = mwResult.significant;
    if (!statSignificant) {
      warnings.push(
        `Not statistically significant (p=${mwResult.p.toFixed(
          3,
        )}) — difference may be noise`,
      );
    }
  }

  return {tooNoisy, statSignificant, warnings};
}

function compareAgainstBaseline(
  received: PerfMetrics,
  baseline: SnapshotEntry,
  threshold: PerfThreshold,
): CompareResult {
  const {tooNoisy, statSignificant, warnings} = checkNoiseGates(
    received,
    baseline,
    threshold,
  );
  const errors: string[] = [];

  // Check duration regression using MEDIAN
  if (!tooNoisy && threshold.maxDurationIncrease !== undefined) {
    const pctChange =
      ((received.medianDuration - baseline.metrics.medianDuration) /
        baseline.metrics.medianDuration) *
      100;
    const absDelta = received.medianDuration - baseline.metrics.medianDuration;
    const minDelta =
      threshold.minAbsoluteDelta ?? DEFAULT_THRESHOLD.minAbsoluteDelta;

    if (
      pctChange > threshold.maxDurationIncrease &&
      absDelta > minDelta &&
      statSignificant
    ) {
      errors.push(
        `Duration regression: +${pctChange.toFixed(1)}% / +${absDelta.toFixed(
          2,
        )}ms ` +
          `(baseline median: ${baseline.metrics.medianDuration.toFixed(
            2,
          )}ms → ` +
          `current median: ${received.medianDuration.toFixed(2)}ms, ` +
          `threshold: ${threshold.maxDurationIncrease}% & ${minDelta}ms)`,
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

  return {errors, warnings};
}

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
    const {file: snapshotFile} = SnapshotManager.getSnapshotPath(testPath);
    const snapshotKey = SnapshotManager.buildKey(testName);

    const isUpdateMode =
      process.argv.includes('-u') || process.argv.includes('--updateSnapshot');

    // Load existing snapshots
    const snapshots = SnapshotManager.load(snapshotFile);
    const baseline = snapshots[snapshotKey];

    const threshold: PerfThreshold = {...DEFAULT_THRESHOLD, ...customThreshold};

    // UPDATE MODE or FIRST RUN: write new baseline
    // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
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
          // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
          `✅ Perf snapshot ${baseline ? 'updated' : 'created'}: ${
            received.name
          }` +
          ` (${received.meanDuration.toFixed(2)}ms, ${
            received.renderCount
          } renders)`,
      };
    }

    // COMPARE MODE
    const {errors, warnings} = compareAgainstBaseline(
      received,
      baseline,
      threshold,
    );
    const isTrackMode = threshold.mode === 'track';
    const warningText =
      warnings.length > 0
        ? '\n' + warnings.map(w => `  ℹ ${w}`).join('\n')
        : '';

    if (isTrackMode && errors.length > 0) {
      return {
        pass: true,
        message: () =>
          `⚠️ [TRACK] "${received.name}" would have failed:\n\n` +
          errors.map(e => `  • ${e}`).join('\n') +
          warningText +
          `\n\n💡 Mode is 'track' — not blocking CI.`,
      };
    }

    if (errors.length > 0) {
      return {
        pass: false,
        message: () =>
          `❌ Performance regression detected in "${received.name}":\n\n` +
          errors.map(e => `  • ${e}`).join('\n') +
          warningText +
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
        `${
          improvement > 0 ? `-${improvement.toFixed(1)}% faster` : 'no change'
        })`,
    };
  },
});
