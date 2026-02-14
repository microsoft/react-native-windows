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
 * A reusable scenario that measures component mount time.
 *
 * Use this when you need a standalone mount scenario outside of
 * ComponentPerfTestBase, or when building composite scenario pipelines.
 *
 * @example
 * ```typescript
 * const mountScenario = new MountScenario(
 *   'View mount',
 *   <View testID="perf-view" style={{ flex: 1 }} />,
 *   { runs: 20 }
 * );
 * const metrics = await mountScenario.run();
 * ```
 */
export class MountScenario implements IScenario {
  readonly name: string;
  readonly description: string;

  constructor(
    name: string,
    private readonly component: React.ReactElement,
    private readonly options: {runs?: number; warmupRuns?: number} = {},
  ) {
    this.name = name;
    this.description = `Mount scenario for ${name}`;
  }

  async run(): Promise<PerfMetrics> {
    return measurePerf(this.component, {
      name: this.name,
      runs: this.options.runs ?? 10,
      warmupRuns: this.options.warmupRuns ?? 1,
    });
  }
}
