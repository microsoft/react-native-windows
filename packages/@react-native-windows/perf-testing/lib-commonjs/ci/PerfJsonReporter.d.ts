/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Custom Jest reporter that writes perf results to a JSON file.
 * Used in CI to produce machine-readable output for downstream comparison
 * and PR comment generation.
 *
 * @format
 */
import type { SnapshotFile } from '../matchers/snapshotManager';
/**
 * Summary of a single test suite's perf results.
 */
export interface SuiteResult {
    testFilePath: string;
    suiteName: string;
    snapshots: SnapshotFile;
    passed: number;
    failed: number;
    totalDuration: number;
}
/**
 * Full CI perf run results.
 */
export interface CIRunResults {
    timestamp: string;
    branch: string;
    commitSha: string;
    suites: SuiteResult[];
    summary: {
        totalSuites: number;
        totalTests: number;
        passed: number;
        failed: number;
        durationMs: number;
    };
}
/**
 * Custom Jest reporter that collects all perf snapshot data
 * after tests complete and writes a single JSON results file.
 *
 * Add to jest config:
 * ```js
 * reporters: [
 *   'default',
 *   ['@react-native-windows/perf-testing/lib-commonjs/ci/PerfJsonReporter', {
 *     outputFile: '.perf-results/results.json'
 *   }]
 * ]
 * ```
 */
export declare class PerfJsonReporter {
    private outputFile;
    constructor(_globalConfig: Record<string, unknown>, options?: {
        outputFile?: string;
    });
    /**
     * Called by Jest after all test suites have completed.
     */
    onRunComplete(_testContexts: Set<unknown>, results: {
        numTotalTestSuites: number;
        numPassedTestSuites: number;
        numFailedTestSuites: number;
        numTotalTests: number;
        numPassedTests: number;
        numFailedTests: number;
        startTime: number;
        testResults: Array<{
            testFilePath: string;
            testResults: Array<{
                ancestorTitles: string[];
                title: string;
                status: string;
                duration?: number;
            }>;
            perfTimer?: {
                start: number;
                end: number;
            };
        }>;
    }): void;
}
