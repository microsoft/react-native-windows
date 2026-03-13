/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {app} from '@react-native-windows/automation';
import {goToComponentExample} from './RNTesterNavigation';
import {
  mean,
  median,
  standardDeviation,
} from '@react-native-windows/perf-testing';
import type {PerfMetrics} from '@react-native-windows/perf-testing';

export interface NativePerfOptions {
  runs?: number;
  warmupRuns?: number;
}

const DEFAULT_RUNS = 15;
const DEFAULT_WARMUP = 2;

export async function navigateToBenchmarkPage(): Promise<void> {
  const componentsTab = await app.findElementByTestID('components-tab');
  await componentsTab.waitForDisplayed({timeout: 120000});

  await goToComponentExample('Native Perf Benchmark');
}

export async function measureNativePerf(
  componentName: string,
  options?: NativePerfOptions,
): Promise<PerfMetrics> {
  const runs = options?.runs ?? DEFAULT_RUNS;
  const warmupRuns = options?.warmupRuns ?? DEFAULT_WARMUP;
  const totalRuns = runs + warmupRuns;

  const componentInput = await app.findElementByTestID('perf-component-input');
  await componentInput.waitForDisplayed({timeout: 5000});

  await app.waitUntil(
    async () => {
      await componentInput.setValue(componentName);
      return (await componentInput.getText()) === componentName;
    },
    {
      interval: 500,
      timeout: 5000,
      timeoutMsg: `Failed to set component: ${componentName}`,
    },
  );

  const runsInput = await app.findElementByTestID('perf-runs-input');
  const runsStr = String(totalRuns);
  await app.waitUntil(
    async () => {
      await runsInput.setValue(runsStr);
      return (await runsInput.getText()) === runsStr;
    },
    {interval: 500, timeout: 5000, timeoutMsg: 'Failed to set run count'},
  );

  const runBtn = await app.findElementByTestID('perf-run-btn');
  await runBtn.waitForDisplayed({timeout: 5000});
  await runBtn.click();

  const statusEl = await app.findElementByTestID('perf-status');
  await app.waitUntil(
    async () => {
      const text = await statusEl.getText();
      return text === 'done';
    },
    {
      interval: 1000,
      timeout: 180000,
      timeoutMsg: `Benchmark timed out for ${componentName}`,
    },
  );

  const resultsEl = await app.findElementByTestID('perf-results');
  const rawJson = await resultsEl.getText();
  const parsed = JSON.parse(rawJson) as {
    componentName: string;
    runs: number;
    durations: number[];
  };

  const durations = parsed.durations.slice(warmupRuns);

  if (durations.length < runs) {
    throw new Error(
      `Expected ${runs} durations after discarding ${warmupRuns} warmup, got ${durations.length}`,
    );
  }

  const meanDuration = mean(durations);
  const medianDuration = median(durations);
  const stdDev = standardDeviation(durations);

  return {
    name: `${componentName} native mount`,
    meanDuration,
    medianDuration,
    stdDev,
    renderCount: 1,
    runs: durations.length,
    durations,
    timestamp: new Date().toISOString(),
    nativeTimings: {fullPipeline: medianDuration},
  };
}
