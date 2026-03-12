/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {app} from '@react-native-windows/automation';
import {ThresholdPresets} from '@react-native-windows/perf-testing';
import {
  navigateToBenchmarkPage,
  measureNativePerf,
} from '../../NativePerfHelpers';

beforeAll(async () => {
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await navigateToBenchmarkPage();
});

const NATIVE = ThresholdPresets.native;

describe('Core Components — Native Render Pipeline', () => {
  test('View native mount', async () => {
    const perf = await measureNativePerf('View', {runs: 15, warmupRuns: 2});
    expect(perf).toMatchPerfSnapshot(NATIVE);
  });

  test('Text native mount', async () => {
    const perf = await measureNativePerf('Text', {runs: 15, warmupRuns: 2});
    expect(perf).toMatchPerfSnapshot(NATIVE);
  });

  test('TextInput native mount', async () => {
    const perf = await measureNativePerf('TextInput', {
      runs: 15,
      warmupRuns: 2,
    });
    expect(perf).toMatchPerfSnapshot(NATIVE);
  });

  test('Image native mount', async () => {
    const perf = await measureNativePerf('Image', {runs: 15, warmupRuns: 2});
    expect(perf).toMatchPerfSnapshot(NATIVE);
  });

  test('Switch native mount', async () => {
    const perf = await measureNativePerf('Switch', {runs: 15, warmupRuns: 2});
    expect(perf).toMatchPerfSnapshot(NATIVE);
  });

  test('ActivityIndicator native mount', async () => {
    const perf = await measureNativePerf('ActivityIndicator', {
      runs: 15,
      warmupRuns: 2,
    });
    expect(perf).toMatchPerfSnapshot(NATIVE);
  });

  test('ScrollView native mount', async () => {
    const perf = await measureNativePerf('ScrollView', {
      runs: 10,
      warmupRuns: 2,
    });
    expect(perf).toMatchPerfSnapshot({...NATIVE, maxCV: 0.6});
  });

  test('FlatList native mount', async () => {
    const perf = await measureNativePerf('FlatList', {
      runs: 10,
      warmupRuns: 2,
    });
    expect(perf).toMatchPerfSnapshot({
      ...NATIVE,
      maxDurationIncrease: 20,
      maxCV: 0.6,
    });
  });
});
