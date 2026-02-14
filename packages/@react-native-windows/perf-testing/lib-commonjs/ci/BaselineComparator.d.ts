/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Compares current perf results against stored baselines.
 * Used in CI to detect regressions and generate comparison reports.
 *
 * @format
 */
import type { PerfMetrics } from '../interfaces/PerfMetrics';
import type { PerfThreshold } from '../interfaces/PerfThreshold';
import type { SnapshotFile } from '../matchers/snapshotManager';
import type { ComparisonResult } from '../reporters/MarkdownReporter';
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
export declare class BaselineComparator {
    /**
     * Compare a single suite's head snapshot against its baseline.
     *
     * @param suiteName - Human-readable suite name (e.g., 'View')
     * @param headSnapshots - Snapshots from the current (PR) run
     * @param baseSnapshots - Snapshots from the baseline (main) branch
     * @param options - Comparison options
     */
    static compareSuite(suiteName: string, headSnapshots: SnapshotFile, baseSnapshots: SnapshotFile, options?: CompareOptions): SuiteComparison;
    /**
     * Compare a single entry's metrics against its baseline.
     */
    static compareEntry(head: PerfMetrics, base: PerfMetrics, threshold: PerfThreshold): ComparisonResult;
}
