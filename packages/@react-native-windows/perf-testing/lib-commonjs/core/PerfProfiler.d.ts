/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as React from 'react';
/**
 * Result from a single profiler measurement.
 */
export interface ProfilerResult {
    /** The phase of the render ('mount' or 'update') */
    phase: 'mount' | 'update' | 'nested-update';
    /** Actual duration of the committed render (ms) */
    actualDuration: number;
    /** Duration of the base render (ms) */
    baseDuration: number;
    /** Start time of the render */
    startTime: number;
    /** Commit time of the render */
    commitTime: number;
}
/**
 * Callback ref for collecting profiler results.
 */
export type OnRenderCallback = (result: ProfilerResult) => void;
/**
 * Props for the PerfProfiler wrapper component.
 */
interface PerfProfilerProps {
    /** Unique identifier for this profiler instance */
    id: string;
    /** Callback invoked on each render commit */
    onResult: OnRenderCallback;
    /** The component tree to profile */
    children: React.ReactNode;
}
/**
 * Wrapper around React.Profiler that collects structured performance data.
 *
 * This component captures render timings using the React Profiler API
 * and forwards them to the measurement engine via the `onResult` callback.
 *
 * @example
 * ```tsx
 * <PerfProfiler id="view-mount" onResult={handleResult}>
 *   <View style={styles.container} />
 * </PerfProfiler>
 * ```
 */
export declare function PerfProfiler({ id, onResult, children, }: PerfProfilerProps): React.ReactElement;
export {};
