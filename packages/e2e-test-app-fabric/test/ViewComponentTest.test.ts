/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  await goToComponentExample('View');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('View Tests', () => {
  test('Views can have border styles', async () => {
    const dump = await dumpVisualTree('border-style-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have offscreen alpha compositing', async () => {
    const dump = await dumpVisualTree('offscreen-alpha-compositing-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have a z-index', async () => {
    const dump = await dumpVisualTree('z-index-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have display: none', async () => {
    const dump = await dumpVisualTree('display-none-button');
    expect(dump).toMatchSnapshot();
  });
  // TODO: Add tests for examples which take testId as a parameter.
});
