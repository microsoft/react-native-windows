/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import type { PerfMetrics } from '../interfaces/PerfMetrics';
import type { PerfThreshold } from '../interfaces/PerfThreshold';
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
 *
 * Snapshot files live in `__perf_snapshots__/` adjacent to the test file
 * and are committed to git, mirroring Jest's visual snapshot pattern.
 */
export declare class SnapshotManager {
    /**
     * Resolve the snapshot directory and file path for a given test file.
     */
    static getSnapshotPath(testFilePath: string): {
        dir: string;
        file: string;
    };
    /**
     * Load an existing snapshot file, or return an empty object.
     */
    static load(snapshotFilePath: string): SnapshotFile;
    /**
     * Save a snapshot file to disk.
     * Creates the directory if it doesn't exist.
     */
    static save(snapshotFilePath: string, snapshots: SnapshotFile): void;
    /**
     * Build a snapshot key from the test name.
     * Follows Jest's `"test name 1"` convention.
     */
    static buildKey(testName: string): string;
}
