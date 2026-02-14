/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import type { PerfMetrics } from './PerfMetrics';
import type { PerfThreshold } from './PerfThreshold';
/**
 * Category for organizing perf tests.
 *
 * - 'core': Built-in RN components (View, Text, TextInput, Image, ScrollView)
 * - 'extended': Higher-level RN components (FlatList, SectionList, RefreshControl)
 * - 'custom': Community or app-specific components
 */
export type PerfTestCategory = 'core' | 'extended' | 'custom';
/**
 * Contract that all component performance tests must implement.
 *
 * This interface enables a consistent API for measuring component
 * performance across core RNW components, extended components, and
 * community-provided custom components.
 *
 * @example
 * ```typescript
 * import { IComponentPerfTest } from '@react-native-windows/perf-testing';
 *
 * class MyComponentPerfTest implements IComponentPerfTest {
 *   readonly componentName = 'MyComponent';
 *   readonly category = 'custom';
 *   readonly testId = 'perf-test-my-component';
 *   readonly defaultThreshold = { maxDurationIncrease: 15 };
 *
 *   async measureMount() { ... }
 *   async measureUnmount() { ... }
 *   async measureRerender() { ... }
 * }
 * ```
 */
export interface IComponentPerfTest {
    /** Component name (e.g., 'View', 'Text', 'MyCustomComponent') */
    readonly componentName: string;
    /** Category for test organization and threshold selection */
    readonly category: PerfTestCategory;
    /** testID used to locate the component in the rendered tree */
    readonly testId: string;
    /** Default threshold for this component's perf tests */
    readonly defaultThreshold: PerfThreshold;
    /** Measure the time to mount the component from scratch */
    measureMount(): Promise<PerfMetrics>;
    /** Measure the time to unmount the component */
    measureUnmount(): Promise<PerfMetrics>;
    /** Measure the time to re-render the component with updated props */
    measureRerender(): Promise<PerfMetrics>;
    /**
     * Return additional component-specific scenarios.
     *
     * Override this to add scenarios like "nested-views-100",
     * "long-text-1000", "scroll-to-end", etc.
     */
    getCustomScenarios?(): IScenario[];
}
/**
 * A single named test scenario that produces performance metrics.
 *
 * Scenarios are the building blocks of perf tests.
 * Use them for custom measurements beyond the standard mount/unmount/rerender.
 *
 * @example
 * ```typescript
 * const nestedViewsScenario: IScenario = {
 *   name: 'nested-views-100',
 *   description: 'Render 100 nested View components',
 *   run: async () => measurePerf(<NestedViews count={100} />, {
 *     name: 'View nested-100',
 *     runs: 5,
 *   }),
 * };
 * ```
 */
export interface IScenario {
    /** Unique scenario name (used as snapshot key) */
    name: string;
    /** Human-readable description */
    description: string;
    /** Execute the scenario and return metrics */
    run(): Promise<PerfMetrics>;
}
