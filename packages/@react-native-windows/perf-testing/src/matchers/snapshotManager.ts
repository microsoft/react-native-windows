/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from '@react-native-windows/fs';
import * as path from 'path';
import * as os from 'os';
import type {PerfMetrics} from '../interfaces/PerfMetrics';
import type {PerfThreshold} from '../interfaces/PerfThreshold';

/**
 * A single entry stored in a `.perf-baseline.json` file.
 */
export interface SnapshotEntry {
  metrics: PerfMetrics;
  threshold: PerfThreshold;
  capturedAt: string;
}

/**
 * A map of snapshot key → entry, representing one `.perf-baseline.json` file.
 */
export type SnapshotFile = Record<string, SnapshotEntry>;

/**
 * Manages reading and writing of perf snapshot files.
 * Also writes live run metrics to a temp directory so the CI reporter
 * (which runs in the Jest main process) can access fresh results
 * from the worker process.
 */
export class SnapshotManager {
  /** Directory for live run metrics (cross-process via temp files). */
  private static readonly _runMetricsDir =
    process.env.PERF_RUN_METRICS_DIR ||
    path.join(os.tmpdir(), 'rnw-perf-run-metrics');

  /** Record a live metric entry for the current run (written to temp file). */
  static recordRunMetric(
    snapshotFilePath: string,
    key: string,
    entry: SnapshotEntry,
  ): void {
    const metricsFile = path.join(
      SnapshotManager._runMetricsDir,
      Buffer.from(snapshotFilePath).toString('base64url') + '.json',
    );
    let existing: SnapshotFile = {};
    if (fs.existsSync(metricsFile)) {
      existing = JSON.parse(
        fs.readFileSync(metricsFile, 'utf-8'),
      ) as SnapshotFile;
    } else if (!fs.existsSync(SnapshotManager._runMetricsDir)) {
      fs.mkdirSync(SnapshotManager._runMetricsDir, {recursive: true});
    }
    existing[key] = entry;
    fs.writeFileSync(
      metricsFile,
      JSON.stringify(existing, null, 2) + '\n',
      'utf-8',
    );
  }

  /** Get live run metrics for a snapshot file, or null if none were recorded. */
  static getRunMetrics(snapshotFilePath: string): SnapshotFile | null {
    const metricsFile = path.join(
      SnapshotManager._runMetricsDir,
      Buffer.from(snapshotFilePath).toString('base64url') + '.json',
    );
    if (fs.existsSync(metricsFile)) {
      return JSON.parse(fs.readFileSync(metricsFile, 'utf-8')) as SnapshotFile;
    }
    return null;
  }

  /** Clean up temp run metrics directory. */
  static clearRunMetrics(): void {
    if (fs.existsSync(SnapshotManager._runMetricsDir)) {
      for (const f of fs.readdirSync(SnapshotManager._runMetricsDir)) {
        fs.unlinkSync(path.join(SnapshotManager._runMetricsDir, f));
      }
    }
  }

  static getSnapshotPath(testFilePath: string): {
    dir: string;
    file: string;
  } {
    const testDir = path.dirname(testFilePath);
    const snapshotDir = path.join(testDir, '__perf_snapshots__');
    const snapshotFile = path.join(
      snapshotDir,
      `${path.basename(testFilePath)}.perf-baseline.json`,
    );
    return {dir: snapshotDir, file: snapshotFile};
  }

  static load(snapshotFilePath: string): SnapshotFile {
    if (fs.existsSync(snapshotFilePath)) {
      const content = fs.readFileSync(snapshotFilePath, 'utf-8');
      const parsed = JSON.parse(content) as SnapshotFile;
      // JSON.stringify turns Infinity into null — restore it on load
      for (const entry of Object.values(parsed)) {
        if ((entry.threshold.maxDuration as unknown) === null) {
          (entry.threshold as {maxDuration: number}).maxDuration = Infinity;
        }
      }
      return parsed;
    }
    return {};
  }

  static save(snapshotFilePath: string, snapshots: SnapshotFile): void {
    const dir = path.dirname(snapshotFilePath);
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, {recursive: true});
    }
    fs.writeFileSync(
      snapshotFilePath,
      JSON.stringify(snapshots, null, 2) + '\n',
      'utf-8',
    );
  }

  // Follows Jest's "test name 1" convention
  static buildKey(testName: string): string {
    return `${testName} 1`;
  }
}
