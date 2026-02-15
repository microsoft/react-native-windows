/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as React from 'react';
import type {PerfMetrics} from '../interfaces/PerfMetrics';
import {mean, median, standardDeviation} from './statistics';
import type {ProfilerResult} from './PerfProfiler';
import {PerfProfiler} from './PerfProfiler';

// React 19 requires all state-updating calls to be wrapped in act().
// We lazy-require react-test-renderer, so we also lazy-resolve act.
let _act: (cb: () => void) => void;
function getAct(): (cb: () => void) => void {
  if (!_act) {
    // eslint-disable-next-line @typescript-eslint/no-var-requires
    const {act} = require('react-test-renderer');
    _act = act;
  }
  return _act;
}

/**
 * Options for the `measurePerf` function.
 */
export interface MeasurePerfOptions {
  /** Scenario name for identification in snapshots and reports */
  name: string;

  /** Number of measurement runs. Default: 10 */
  runs?: number;

  /** Warmup runs that are not recorded. Default: 1 */
  warmupRuns?: number;

  /**
   * Optional async callback executed after each render.
   * Use this to simulate user interactions (scroll, type, press).
   *
   * @param helpers - Render helpers providing access to the rendered tree.
   */
  scenario?: (helpers: RenderHelpers) => Promise<void>;

  /** Whether to measure unmount time instead of mount time. Default: false */
  measureUnmount?: boolean;
}

/**
 * Helpers passed to scenario callbacks during measurement.
 */
export interface RenderHelpers {
  /**
   * Trigger a re-render by updating the component.
   * Implementations should call `rerender()` from the test renderer.
   */
  rerender: (element: React.ReactElement) => void;

  /**
   * Unmount the rendered component tree.
   */
  unmount: () => void;
}

/**
 * Result from a single measurement iteration.
 */
interface SingleRunResult {
  duration: number;
  renderCount: number;
}

/**
 * Core measurement function.
 *
 * Renders a component multiple times wrapped in React.Profiler,
 * collects timing data, and returns aggregated metrics.
 *
 * This function is the foundation of all perf tests. Both the base class
 * and individual scenarios use this to collect measurements.
 *
 * @param component - The React element to measure.
 * @param options - Configuration for the measurement run.
 * @returns Aggregated performance metrics.
 *
 * @example
 * ```typescript
 * const metrics = await measurePerf(
 *   <View testID="perf-view" style={{ flex: 1 }} />,
 *   { name: 'View mount', runs: 10 }
 * );
 * expect(metrics).toMatchPerfSnapshot();
 * ```
 */
export async function measurePerf(
  component: React.ReactElement,
  options: MeasurePerfOptions,
): Promise<PerfMetrics> {
  const {
    name,
    runs = 10,
    warmupRuns = 1,
    scenario,
    measureUnmount = false,
  } = options;

  // Lazy-require react-test-renderer to keep it as a peerDep
  // eslint-disable-next-line @typescript-eslint/no-var-requires
  const TestRenderer = require('react-test-renderer');

  const durations: number[] = [];
  let totalRenderCount = 0;

  // Warmup runs — not recorded, stabilize JIT/caches
  for (let i = 0; i < warmupRuns; i++) {
    await runSingleMeasurement(
      TestRenderer,
      component,
      `${name}-warmup-${i}`,
      scenario,
      measureUnmount,
    );
  }

  // Actual measurement runs
  for (let i = 0; i < runs; i++) {
    const result = await runSingleMeasurement(
      TestRenderer,
      component,
      `${name}-run-${i}`,
      scenario,
      measureUnmount,
    );
    durations.push(result.duration);
    totalRenderCount += result.renderCount;
  }

  return {
    name,
    meanDuration: mean(durations),
    medianDuration: median(durations),
    stdDev: standardDeviation(durations),
    renderCount: Math.round(totalRenderCount / runs),
    runs,
    timestamp: new Date().toISOString(),
  };
}

/**
 * Execute a single measurement iteration.
 */
async function runSingleMeasurement(
  TestRenderer: typeof import('react-test-renderer'),
  component: React.ReactElement,
  profilerId: string,
  scenario?: (helpers: RenderHelpers) => Promise<void>,
  measureUnmount: boolean = false,
): Promise<SingleRunResult> {
  const results: ProfilerResult[] = [];

  const onResult = (result: ProfilerResult) => {
    results.push(result);
  };

  const wrappedComponent = React.createElement(PerfProfiler, {
    id: profilerId,
    onResult,
    children: component,
  });

  let renderer!: ReturnType<typeof TestRenderer.create>;

  const act = getAct();

  if (measureUnmount) {
    // Mount first (not measured)
    act(() => {
      renderer = TestRenderer.create(wrappedComponent);
    });

    // Clear results from mount phase
    results.length = 0;

    // Measure unmount
    const unmountStart = performance.now();
    act(() => {
      renderer.unmount();
    });
    const unmountDuration = performance.now() - unmountStart;

    return {
      duration: unmountDuration,
      renderCount: 0,
    };
  }

  // Measure mount
  const mountStart = performance.now();
  act(() => {
    renderer = TestRenderer.create(wrappedComponent);
  });
  const mountDuration = performance.now() - mountStart;

  // Execute custom scenario if provided
  if (scenario) {
    const helpers: RenderHelpers = {
      rerender: (element: React.ReactElement) => {
        const wrappedUpdate = React.createElement(PerfProfiler, {
          id: profilerId,
          onResult,
          children: element,
        });
        act(() => {
          renderer.update(wrappedUpdate);
        });
      },
      unmount: () => {
        act(() => {
          renderer.unmount();
        });
      },
    };
    await scenario(helpers);
  }

  // Clean up
  if (renderer) {
    try {
      act(() => {
        renderer.unmount();
      });
    } catch {
      // Already unmounted by scenario
    }
  }

  // Use Profiler actualDuration if available, fallback to manual timing
  const totalActualDuration = results.reduce(
    (sum, r) => sum + r.actualDuration,
    0,
  );

  return {
    duration: totalActualDuration > 0 ? totalActualDuration : mountDuration,
    renderCount: results.length,
  };
}
