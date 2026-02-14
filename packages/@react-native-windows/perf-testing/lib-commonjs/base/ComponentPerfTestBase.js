"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.ComponentPerfTestBase = void 0;
const measurePerf_1 = require("../core/measurePerf");
const PerfThreshold_1 = require("../interfaces/PerfThreshold");
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
class ComponentPerfTestBase {
    /**
     * Override to customize thresholds for this component.
     * Defaults to the global DEFAULT_THRESHOLD.
     */
    get defaultThreshold() {
        return PerfThreshold_1.DEFAULT_THRESHOLD;
    }
    // ─── Standard Scenarios ───
    async measureMount() {
        var _a;
        return (0, measurePerf_1.measurePerf)(this.createComponent(), {
            name: `${this.componentName} mount`,
            runs: (_a = this.defaultThreshold.minRuns) !== null && _a !== void 0 ? _a : 10,
            warmupRuns: 1,
        });
    }
    async measureUnmount() {
        var _a;
        return (0, measurePerf_1.measurePerf)(this.createComponent(), {
            name: `${this.componentName} unmount`,
            runs: (_a = this.defaultThreshold.minRuns) !== null && _a !== void 0 ? _a : 10,
            warmupRuns: 1,
            measureUnmount: true,
        });
    }
    async measureRerender() {
        var _a;
        const component = this.createComponent();
        return (0, measurePerf_1.measurePerf)(component, {
            name: `${this.componentName} rerender`,
            runs: (_a = this.defaultThreshold.minRuns) !== null && _a !== void 0 ? _a : 10,
            warmupRuns: 1,
            scenario: async (helpers) => {
                // Default re-render: update with same element (forces reconciliation)
                helpers.rerender(this.createComponent({ key: Date.now() }));
            },
        });
    }
    // ─── Extension Point ───
    /**
     * Override to add component-specific scenarios.
     * Return an empty array if there are no custom scenarios.
     */
    getCustomScenarios() {
        return [];
    }
    // ─── Test Suite Generator ───
    /**
     * Generates a complete Jest test suite for this component.
     *
     * Call this from your `.perf-test.tsx` file to automatically create
     * describe/test blocks for all standard and custom scenarios.
     *
     * @param customThreshold - Optional threshold override for snapshot matching.
     */
    generateTestSuite(customThreshold) {
        const self = this;
        describe(`${this.componentName} Performance`, () => {
            test('mount time', async () => {
                const perf = await self.measureMount();
                expect(perf).toMatchPerfSnapshot(customThreshold);
            });
            test('unmount time', async () => {
                const perf = await self.measureUnmount();
                expect(perf).toMatchPerfSnapshot(customThreshold);
            });
            test('rerender time', async () => {
                const perf = await self.measureRerender();
                expect(perf).toMatchPerfSnapshot(customThreshold);
            });
            // Custom scenarios
            const customScenarios = self.getCustomScenarios();
            if (customScenarios.length > 0) {
                describe(`${self.componentName}-Specific Scenarios`, () => {
                    for (const scenario of customScenarios) {
                        test(scenario.name, async () => {
                            const perf = await scenario.run();
                            expect(perf).toMatchPerfSnapshot(customThreshold);
                        });
                    }
                });
            }
        });
    }
}
exports.ComponentPerfTestBase = ComponentPerfTestBase;
//# sourceMappingURL=ComponentPerfTestBase.js.map