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
 * Subclasses provide componentName, category, testId, and createComponent().
 * The base class handles standard mount/unmount/rerender scenarios and
 * test suite generation.
 */
export abstract class ComponentPerfTestBase implements IComponentPerfTest {
  abstract readonly componentName: string;
  abstract readonly category: PerfTestCategory;
  abstract readonly testId: string;

  get defaultThreshold(): PerfThreshold {
    return DEFAULT_THRESHOLD;
  }

  abstract createComponent(
    props?: Record<string, unknown>,
  ): React.ReactElement;

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
        // Default re-render: update with a changed prop to force reconciliation
        helpers.rerender(this.createComponent({__perfRerenderKey: Date.now()}));
      },
    });
  }

  /**
   * Override to add component-specific scenarios.
   */
  getCustomScenarios(): IScenario[] {
    return [];
  }

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
