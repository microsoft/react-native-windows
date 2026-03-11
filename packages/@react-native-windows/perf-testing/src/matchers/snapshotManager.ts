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
 */
export class SnapshotManager {
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

  /**
   * Path to the temp file where the current run's measured values are stored.
   * Written by toMatchPerfSnapshot (worker), read by PerfJsonReporter (main).
   */
  static getCurrentRunPath(testFilePath: string): string {
    const testDir = path.dirname(testFilePath);
    const snapshotDir = path.join(testDir, '__perf_snapshots__');
    return path.join(
      snapshotDir,
      `${path.basename(testFilePath)}.perf-current.json`,
    );
  }

  /**
   * Record a single test's measured values to the current-run temp file.
   * Called from the test worker process by toMatchPerfSnapshot.
   */
  static recordCurrentRun(
    testFilePath: string,
    key: string,
    entry: SnapshotEntry,
  ): void {
    const currentPath = this.getCurrentRunPath(testFilePath);
    const existing = this.load(currentPath);
    existing[key] = entry;
    this.save(currentPath, existing);
  }

  /**
   * Load the current run's measured values from the temp file.
   * Called from the main process by PerfJsonReporter.
   */
  static loadCurrentRun(testFilePath: string): SnapshotFile {
    return this.load(this.getCurrentRunPath(testFilePath));
  }

  /**
   * Remove the current-run temp file (cleanup after reporting).
   */
  static cleanCurrentRun(testFilePath: string): void {
    const p = this.getCurrentRunPath(testFilePath);
    if (fs.existsSync(p)) {
      fs.unlinkSync(p);
    }
  }
}
