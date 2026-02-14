/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as React from 'react';
import type { IScenario } from '../interfaces/IComponentPerfTest';
import type { PerfMetrics } from '../interfaces/PerfMetrics';
import type { RenderHelpers } from '../core/measurePerf';
/**
 * A reusable scenario that measures component re-render time.
 *
 * Accepts an optional `triggerRerender` callback to simulate
 * prop changes or state updates specific to the component.
 *
 * @example
 * ```typescript
 * const rerenderScenario = new RerenderScenario(
 *   'Text rerender',
 *   <Text testID="perf-text">Hello</Text>,
 *   {
 *     triggerRerender: (helpers) => {
 *       helpers.rerender(<Text testID="perf-text">Updated</Text>);
 *     },
 *   }
 * );
 * const metrics = await rerenderScenario.run();
 * ```
 */
export declare class RerenderScenario implements IScenario {
    private readonly component;
    private readonly options;
    readonly name: string;
    readonly description: string;
    constructor(name: string, component: React.ReactElement, options?: {
        runs?: number;
        warmupRuns?: number;
        triggerRerender?: (helpers: RenderHelpers) => void | Promise<void>;
    });
    run(): Promise<PerfMetrics>;
}
