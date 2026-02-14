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

/**
 * A reusable scenario that measures component unmount time.
 *
 * @example
 * ```typescript
 * const unmountScenario = new UnmountScenario(
 *   'View unmount',
 *   <View testID="perf-view" style={{ flex: 1 }} />,
 * );
 * const metrics = await unmountScenario.run();
 * ```
 */
export class UnmountScenario implements IScenario {
  readonly name: string;
  readonly description: string;

  constructor(
    name: string,
    private readonly component: React.ReactElement,
    private readonly options: {runs?: number; warmupRuns?: number} = {},
  ) {
    this.name = name;
    this.description = `Unmount scenario for ${name}`;
  }

  async run(): Promise<PerfMetrics> {
    return measurePerf(this.component, {
      name: this.name,
      runs: this.options.runs ?? 10,
      warmupRuns: this.options.warmupRuns ?? 1,
      measureUnmount: true,
    });
  }
}
