/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {app} from '@react-native-windows/automation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Custom Native Accessibility Example');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Custom Accessibility Tests', () => {
  test('Verify custom native component has UIA label from native', async () => {
    const nativeComponent = await dumpVisualTree('custom-accessibility-1');

    // Verify that the native component reports its accessiblity label from the native code
    expect(nativeComponent['Automation Tree'].Name).toBe(
      'accessiblity label from native',
    );

    const dump = await dumpVisualTree('custom-accessibility-root-1');
    expect(dump).toMatchSnapshot();
  });
});
