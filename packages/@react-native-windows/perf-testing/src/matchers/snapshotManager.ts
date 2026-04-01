/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from '@react-native-windows/fs';
import * as path from 'path';
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
 * Also maintains an in-memory registry of current-run metrics
 * so the CI reporter can access live results
 */
export class SnapshotManager {
  /** In-memory store of metrics captured during the current test run. */
  private static readonly _runMetrics: Map<string, SnapshotFile> = new Map();
  static recordRunMetric(
    snapshotFilePath: string,
    key: string,
    entry: SnapshotEntry,
  ): void {
    if (!SnapshotManager._runMetrics.has(snapshotFilePath)) {
      SnapshotManager._runMetrics.set(snapshotFilePath, {});
    }
    SnapshotManager._runMetrics.get(snapshotFilePath)![key] = entry;
  }

  static getRunMetrics(snapshotFilePath: string): SnapshotFile | null {
    return SnapshotManager._runMetrics.get(snapshotFilePath) ?? null;
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
