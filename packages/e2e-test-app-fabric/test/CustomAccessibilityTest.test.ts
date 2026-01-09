/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToApiExample} from './RNTesterNavigation';
import {app} from '@react-native-windows/automation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToApiExample('CustomAccessibilityExample');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Custom Accessibility Tests', () => {
  test('Verify custom native component has UIA label from native', async () => {
    //const root = await app.findElementByTestID('custom-accessibility-root-1');
    const dump = await dumpVisualTree('custom-accessibility-root-1');

    expect(dump).toMatchSnapshot();
  });
});
