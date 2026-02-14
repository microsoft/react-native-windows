/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as React from 'react';
import {measurePerf} from '../core/measurePerf';
import type {
  IComponentPerfTest,
  IScenario,
  PerfTestCategory,
} from '../interfaces/IComponentPerfTest';
import type {PerfMetrics} from '../interfaces/PerfMetrics';
import type {PerfThreshold} from '../interfaces/PerfThreshold';
import {DEFAULT_THRESHOLD} from '../interfaces/PerfThreshold';

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
export abstract class ComponentPerfTestBase implements IComponentPerfTest {
  abstract readonly componentName: string;
  abstract readonly category: PerfTestCategory;
  abstract readonly testId: string;

  /**
   * Override to customize thresholds for this component.
   * Defaults to the global DEFAULT_THRESHOLD.
   */
  get defaultThreshold(): PerfThreshold {
    return DEFAULT_THRESHOLD;
  }

  /**
   * Create the React element to be measured.
   * Override this in every subclass.
   *
   * @param props - Optional additional props to pass to the component.
   */
  abstract createComponent(
    props?: Record<string, unknown>,
  ): React.ReactElement;

  // ─── Standard Scenarios ───

  async measureMount(): Promise<PerfMetrics> {
    return measurePerf(this.createComponent(), {
      name: `${this.componentName} mount`,
      runs: this.defaultThreshold.minRuns ?? 10,
      warmupRuns: 1,
    });
  }

  async measureUnmount(): Promise<PerfMetrics> {
    return measurePerf(this.createComponent(), {
      name: `${this.componentName} unmount`,
      runs: this.defaultThreshold.minRuns ?? 10,
      warmupRuns: 1,
      measureUnmount: true,
    });
  }

  async measureRerender(): Promise<PerfMetrics> {
    const component = this.createComponent();
    return measurePerf(component, {
      name: `${this.componentName} rerender`,
      runs: this.defaultThreshold.minRuns ?? 10,
      warmupRuns: 1,
      scenario: async helpers => {
        // Default re-render: update with same element (forces reconciliation)
        helpers.rerender(this.createComponent({key: Date.now()}));
      },
    });
  }

  // ─── Extension Point ───

  /**
   * Override to add component-specific scenarios.
   * Return an empty array if there are no custom scenarios.
   */
  getCustomScenarios(): IScenario[] {
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
  generateTestSuite(customThreshold?: Partial<PerfThreshold>): void {
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
