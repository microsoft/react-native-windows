/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { dumpVisualTree } from '@react-native-windows/automation-commands';
import { goToApiExample } from './RNTesterNavigation';
import { verifyNoErrorLogs } from './Helpers';
import { app } from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToApiExample('Pointer Clicks');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});


describe('Pointer onClick Test', () => {
  test('onClick reports correct native event properties on left click', async () => {
    const component = await app.findElementByTestID('pointer-click-target');
    await component.waitForDisplayed({ timeout: 5000 });

    // Left click triggers onPointerDown with button=0
    await component.click();
    const stateText = await app.findElementByTestID('pointer-click-text');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('"button": 0') && currentText.includes('onClick');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onClick with button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toMatchSnapshot();
  });
  test('onAuxClick reports correct native event properties on middle click', async () => {
    const component = await app.findElementByTestID('pointer-click-target');
    await component.waitForDisplayed({ timeout: 5000 });

    // Middle click triggers onPointerDown with button=1
    await component.click({ button: 'middle' });
    const stateText = await app.findElementByTestID('pointer-click-text');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('"button": 1') && currentText.includes('onAuxClick');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onAuxClick with middle button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toMatchSnapshot();
  });
  test('onAuxClick reports correct native event properties on right click', async () => {
    const component = await app.findElementByTestID('pointer-click-target');
    await component.waitForDisplayed({ timeout: 5000 });

    // Middle click triggers onPointerDown with button=2
    await component.click({ button: 'right' });
    const stateText = await app.findElementByTestID('pointer-click-text');

    await app.waitUntil(
      async () => {
        const currentText = await stateText.getText();
        return currentText.includes('"button": 2') && currentText.includes('onAuxClick');
      },
      {
        timeout: 5000,
        timeoutMsg:
          'State text not updated after onAuxClick with right button property.',
      },
    );

    const text = await stateText.getText();
    expect(text).toMatchSnapshot();
  });

});
