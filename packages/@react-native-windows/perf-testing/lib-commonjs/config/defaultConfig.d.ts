/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import type { PerfThreshold } from '../interfaces/PerfThreshold';
/**
 * Global perf-testing configuration.
 */
export interface PerfTestConfig {
    /** Default number of measurement runs */
    defaultRuns: number;
    /** Default warmup runs */
    defaultWarmupRuns: number;
    /** Default threshold applied to all tests */
    defaultThreshold: Required<PerfThreshold>;
    /** Whether to output verbose logs during measurement */
    verbose: boolean;
    /** Output directory for JSON results (used by CI) */
    outputDir: string;
}
/**
 * Default configuration for all perf tests.
 */
export declare const DEFAULT_CONFIG: Readonly<PerfTestConfig>;
/**
 * Create a custom config by merging overrides with defaults.
 */
export declare function createPerfConfig(overrides: Partial<PerfTestConfig>): PerfTestConfig;
