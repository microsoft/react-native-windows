/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as React from 'react';
import type { IScenario } from '../interfaces/IComponentPerfTest';
import type { PerfMetrics } from '../interfaces/PerfMetrics';
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
export declare class MountScenario implements IScenario {
    private readonly component;
    private readonly options;
    readonly name: string;
    readonly description: string;
    constructor(name: string, component: React.ReactElement, options?: {
        runs?: number;
        warmupRuns?: number;
    });
    run(): Promise<PerfMetrics>;
}
