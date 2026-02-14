/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as React from 'react';
import type { IComponentPerfTest, IScenario, PerfTestCategory } from '../interfaces/IComponentPerfTest';
import type { PerfMetrics } from '../interfaces/PerfMetrics';
import type { PerfThreshold } from '../interfaces/PerfThreshold';
/**
 * Abstract base class for all component performance tests.
 *
 * Extend this class to create perf tests for any React Native component —
 * whether it's a core RNW component, an extended component like RefreshControl,
 * or a community-provided custom native component.
 *
 * The base class provides:
 * - Standard mount/unmount/rerender scenarios out of the box
 * - A `getCustomScenarios()` hook for component-specific measurements
 * - A `generateTestSuite()` helper that creates a full Jest describe/test block
 * - Default thresholds that can be overridden per-component
 *
 * @example
 * ```typescript
 * // Community usage for a custom native component:
 * class MySliderPerfTest extends ComponentPerfTestBase {
 *   readonly componentName = 'MySlider';
 *   readonly category = 'custom';
 *   readonly testId = 'perf-test-my-slider';
 *
 *   createComponent(props?: Record<string, unknown>) {
 *     return <MySlider testID={this.testId} {...props} />;
 *   }
 * }
 * ```
 *
 * @example
 * ```typescript
 * // RNW internal usage for a new component:
 * class RefreshControlPerfTest extends ComponentPerfTestBase {
 *   readonly componentName = 'RefreshControl';
 *   readonly category = 'extended';
 *   readonly testId = 'perf-test-refreshcontrol';
 *
 *   createComponent(props?: Record<string, unknown>) {
 *     return (
 *       <ScrollView
 *         refreshControl={
 *           <RefreshControl testID={this.testId} refreshing={false} {...props} />
 *         }
 *       />
 *     );
 *   }
 *
 *   getCustomScenarios(): IScenario[] {
 *     return [{
 *       name: 'refresh-animation',
 *       description: 'Measure refresh animation cycle',
 *       run: () => this.measureRefreshAnimation(),
 *     }];
 *   }
 * }
 * ```
 */
export declare abstract class ComponentPerfTestBase implements IComponentPerfTest {
    abstract readonly componentName: string;
    abstract readonly category: PerfTestCategory;
    abstract readonly testId: string;
    /**
     * Override to customize thresholds for this component.
     * Defaults to the global DEFAULT_THRESHOLD.
     */
    get defaultThreshold(): PerfThreshold;
    /**
     * Create the React element to be measured.
     * Override this in every subclass.
     *
     * @param props - Optional additional props to pass to the component.
     */
    abstract createComponent(props?: Record<string, unknown>): React.ReactElement;
    measureMount(): Promise<PerfMetrics>;
    measureUnmount(): Promise<PerfMetrics>;
    measureRerender(): Promise<PerfMetrics>;
    /**
     * Override to add component-specific scenarios.
     * Return an empty array if there are no custom scenarios.
     */
    getCustomScenarios(): IScenario[];
    /**
     * Generates a complete Jest test suite for this component.
     *
     * Call this from your `.perf-test.tsx` file to automatically create
     * describe/test blocks for all standard and custom scenarios.
     *
     * @param customThreshold - Optional threshold override for snapshot matching.
     */
    generateTestSuite(customThreshold?: Partial<PerfThreshold>): void;
}
