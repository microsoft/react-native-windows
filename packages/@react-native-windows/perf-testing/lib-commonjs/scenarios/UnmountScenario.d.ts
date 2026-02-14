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
export declare class UnmountScenario implements IScenario {
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
