/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import type { PerfMetrics } from '../interfaces/PerfMetrics';
/**
 * Reporter that outputs perf results to the console.
 *
 * Used as the default reporter during local development.
 */
export declare class ConsoleReporter {
    /**
     * Print a summary of multiple perf results.
     */
    static report(results: PerfMetrics[]): void;
    /**
     * Print a single result line.
     */
    static reportSingle(result: PerfMetrics): void;
}
