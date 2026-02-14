/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as React from 'react';
import type { PerfMetrics } from '../interfaces/PerfMetrics';
/**
 * Options for the `measurePerf` function.
 */
export interface MeasurePerfOptions {
    /** Scenario name for identification in snapshots and reports */
    name: string;
    /** Number of measurement runs. Default: 10 */
    runs?: number;
    /** Warmup runs that are not recorded. Default: 1 */
    warmupRuns?: number;
    /**
     * Optional async callback executed after each render.
     * Use this to simulate user interactions (scroll, type, press).
     *
     * @param helpers - Render helpers providing access to the rendered tree.
     */
    scenario?: (helpers: RenderHelpers) => Promise<void>;
    /** Whether to measure unmount time instead of mount time. Default: false */
    measureUnmount?: boolean;
}
/**
 * Helpers passed to scenario callbacks during measurement.
 */
export interface RenderHelpers {
    /**
     * Trigger a re-render by updating the component.
     * Implementations should call `rerender()` from the test renderer.
     */
    rerender: (element: React.ReactElement) => void;
    /**
     * Unmount the rendered component tree.
     */
    unmount: () => void;
}
/**
 * Core measurement function.
 *
 * Renders a component multiple times wrapped in React.Profiler,
 * collects timing data, and returns aggregated metrics.
 *
 * This function is the foundation of all perf tests. Both the base class
 * and individual scenarios use this to collect measurements.
 *
 * @param component - The React element to measure.
 * @param options - Configuration for the measurement run.
 * @returns Aggregated performance metrics.
 *
 * @example
 * ```typescript
 * const metrics = await measurePerf(
 *   <View testID="perf-view" style={{ flex: 1 }} />,
 *   { name: 'View mount', runs: 10 }
 * );
 * expect(metrics).toMatchPerfSnapshot();
 * ```
 */
export declare function measurePerf(component: React.ReactElement, options: MeasurePerfOptions): Promise<PerfMetrics>;
