/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as React from 'react';
import type {IScenario} from '../interfaces/IComponentPerfTest';
import type {PerfMetrics} from '../interfaces/PerfMetrics';
import {measurePerf} from '../core/measurePerf';
import type {RenderHelpers} from '../core/measurePerf';

/**
 * Reusable scenario that measures component re-render time.
 */
export class RerenderScenario implements IScenario {
  readonly name: string;
  readonly description: string;

  constructor(
    name: string,
    private readonly component: React.ReactElement,
    private readonly options: {
      runs?: number;
      warmupRuns?: number;
      triggerRerender?: (helpers: RenderHelpers) => void | Promise<void>;
    } = {},
  ) {
    this.name = name;
    this.description = `Rerender scenario for ${name}`;
  }

  async run(): Promise<PerfMetrics> {
    return measurePerf(this.component, {
      name: this.name,
      runs: this.options.runs ?? 10,
      warmupRuns: this.options.warmupRuns ?? 1,
      scenario: async helpers => {
        if (this.options.triggerRerender) {
          await this.options.triggerRerender(helpers);
        } else {
          // Default: re-render the same component (forces reconciliation)
          helpers.rerender(this.component);
        }
      },
    });
  }
}
