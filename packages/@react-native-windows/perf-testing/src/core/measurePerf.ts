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
  name: string;
  runs?: number;
  warmupRuns?: number;
  scenario?: (helpers: RenderHelpers) => Promise<void>;
  measureUnmount?: boolean;
}

/**
 * Helpers passed to scenario callbacks during measurement.
 */
export interface RenderHelpers {
  rerender: (element: React.ReactElement) => void;
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

  for (let i = 0; i < warmupRuns; i++) {
    await runSingleMeasurement(
      TestRenderer,
      component,
      `${name}-warmup-${i}`,
      scenario,
      measureUnmount,
    );
  }

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
    act(() => {
      renderer = TestRenderer.create(wrappedComponent);
    });

    results.length = 0;

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
      // Already unmounted
    }
  }

  // Prefer Profiler actualDuration, fallback to manual timing
  const totalActualDuration = results.reduce(
    (sum, r) => sum + r.actualDuration,
    0,
  );

  return {
    duration: totalActualDuration > 0 ? totalActualDuration : mountDuration,
    renderCount: results.length,
  };
}
