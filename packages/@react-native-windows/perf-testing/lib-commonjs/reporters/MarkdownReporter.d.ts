/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import type { PerfMetrics } from '../interfaces/PerfMetrics';
/**
 * Comparison info used when generating markdown reports.
 */
export interface ComparisonResult {
    metrics: PerfMetrics;
    baselineMetrics?: PerfMetrics;
    percentChange?: number;
    passed: boolean;
    error?: string;
}
/**
 * Reporter that generates Markdown-formatted perf results.
 *
 * Used for CI/CD PR comments and artifact reports.
 */
export declare class MarkdownReporter {
    /**
     * Generate a full markdown report from comparison results.
     */
    static generate(comparisons: ComparisonResult[]): string;
    /**
     * Generate a short summary line for use in PR comments.
     */
    static summary(comparisons: ComparisonResult[]): string;
}
