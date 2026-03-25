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

describe('List Components — Native Render Pipeline', () => {
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
