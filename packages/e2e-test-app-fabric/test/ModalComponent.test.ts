/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {dumpVisualTree} from '@react-native-windows/automation-commands';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Modal');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('Modal Tests', () => {
  // very basic test of making sure Modal doesn't crash the app after opening/closing
  test('You can open/close a Modal', async () => {
    const modalOpenButton = await app.findElementByTestID('modal_open_button');
    await modalOpenButton.waitForDisplayed({timeout: 20000});
    // Modal should open 
    await modalOpenButton.click();
    // Modal should close
    await modalOpenButton.click();
    await modalOpenButton.waitForDisplayed({timeout: 20000});
  });
});
