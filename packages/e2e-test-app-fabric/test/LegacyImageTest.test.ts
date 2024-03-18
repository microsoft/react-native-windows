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
  await goToComponentExample('LegacyImageTest');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('LegacyImageTest', () => {
  /* Test case #1: view and image displayed with no border and cornerRadius */
  test('ImageWithoutBorderTest', async () => {
    const dump = await dumpVisualTree('image-container');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #2: Click button once, update view and image with round border*/
  test('ImageWithBorderTest', async () => {
    await toggleImageBorder();
    const dump = await dumpVisualTree('image-container');
    expect(dump).toMatchSnapshot();
  });

  /* Test case #3: Click button one more, remove border from view and image but tree structure is different from #1*/
  test('ImageWithoutBorderTestOneMoreClick', async () => {
    await toggleImageBorder();
    const dump = await dumpVisualTree('image-container');
    expect(dump).toMatchSnapshot();
  });

  test('ImageRTLTest', async () => {
    await toggleRTLMode();
    const dump = await dumpVisualTree('image-container');
    expect(dump).toMatchSnapshot();
  });
});

async function toggleImageBorder() {
  const imageBorderToggle = await app.findElementByTestID(
    'toggle-border-button',
  );
  await imageBorderToggle.click();
}

async function toggleRTLMode() {
  const rtlToggleButton = await app.findElementByTestID('set-rtl-button');
  await rtlToggleButton.click();
}
