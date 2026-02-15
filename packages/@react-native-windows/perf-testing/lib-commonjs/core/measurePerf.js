"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.measurePerf = void 0;
const React = __importStar(require("react"));
const statistics_1 = require("./statistics");
const PerfProfiler_1 = require("./PerfProfiler");
// React 19 requires all state-updating calls to be wrapped in act().
// We lazy-require react-test-renderer, so we also lazy-resolve act.
let _act;
function getAct() {
    if (!_act) {
        // eslint-disable-next-line @typescript-eslint/no-var-requires
        const { act } = require('react-test-renderer');
        _act = act;
    }
    return _act;
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
async function measurePerf(component, options) {
    const { name, runs = 10, warmupRuns = 1, scenario, measureUnmount = false, } = options;
    // Lazy-require react-test-renderer to keep it as a peerDep
    // eslint-disable-next-line @typescript-eslint/no-var-requires
    const TestRenderer = require('react-test-renderer');
    const durations = [];
    let totalRenderCount = 0;
    // Warmup runs — not recorded, stabilize JIT/caches
    for (let i = 0; i < warmupRuns; i++) {
        await runSingleMeasurement(TestRenderer, component, `${name}-warmup-${i}`, scenario, measureUnmount);
    }
    // Actual measurement runs
    for (let i = 0; i < runs; i++) {
        const result = await runSingleMeasurement(TestRenderer, component, `${name}-run-${i}`, scenario, measureUnmount);
        durations.push(result.duration);
        totalRenderCount += result.renderCount;
    }
    return {
        name,
        meanDuration: (0, statistics_1.mean)(durations),
        medianDuration: (0, statistics_1.median)(durations),
        stdDev: (0, statistics_1.standardDeviation)(durations),
        renderCount: Math.round(totalRenderCount / runs),
        runs,
        timestamp: new Date().toISOString(),
    };
}
exports.measurePerf = measurePerf;
/**
 * Execute a single measurement iteration.
 */
async function runSingleMeasurement(TestRenderer, component, profilerId, scenario, measureUnmount = false) {
    const results = [];
    const onResult = (result) => {
        results.push(result);
    };
    const wrappedComponent = React.createElement(PerfProfiler_1.PerfProfiler, {
        id: profilerId,
        onResult,
        children: component,
    });
    let renderer;
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
        const helpers = {
            rerender: (element) => {
                const wrappedUpdate = React.createElement(PerfProfiler_1.PerfProfiler, {
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
        }
        catch (_a) {
            // Already unmounted by scenario
        }
    }
    // Use Profiler actualDuration if available, fallback to manual timing
    const totalActualDuration = results.reduce((sum, r) => sum + r.actualDuration, 0);
    return {
        duration: totalActualDuration > 0 ? totalActualDuration : mountDuration,
        renderCount: results.length,
    };
}
//# sourceMappingURL=measurePerf.js.map