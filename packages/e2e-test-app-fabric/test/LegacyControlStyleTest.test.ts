/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('LegacyControlStyleTest');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('LegacyControlStyleTest', () => {
  /* Test case #1: Controls style with regular border */
  test('ControlStyleTestWithRegularBorder', async () => {
    const dump = await dumpVisualTree('control-style-switch-view');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #2: Click button once, update controls style and round border*/
  test('ControlStyleTestWithRoundBorder', async () => {
    await toggleControlBorder();
    const dump = await dumpVisualTree('control-style-switch-view');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #3: Click button one more, return to #1*/
  test('ControlStyleTestWithRegularBorder #2', async () => {
    await toggleControlBorder();
    const dump = await dumpVisualTree('control-style-switch-view');
    expect(dump).toMatchSnapshot();
  });
});

async function toggleControlBorder() {
  const showBorderToggle = await app.findElementByTestID('show-border-toggle');
  await showBorderToggle.click();
}
