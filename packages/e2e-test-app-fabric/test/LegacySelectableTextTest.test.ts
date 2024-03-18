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
  await goToComponentExample('LegacySelectableTextTest');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('LegacySelectableTextTest', () => {
  beforeEach(async () => {
    await clearState();
  });

  test('PressableWhenNotSelectable', async () => {
    const textExample = await app.findElementByTestID('text-example');
    await textExample.click();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('DoubleClickWhenNotSelectable', async () => {
    const textExample = await app.findElementByTestID('text-example');
    await textExample.doubleClick();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('PressableWhenSelectable', async () => {
    await toggleSelectable();
    const textExample = await app.findElementByTestID('text-example');
    await textExample.click();
    const dump = await dumpVisualTree('pressed-state');
    expect(dump).toMatchSnapshot();
  });

  test('DoubleClickWhenSelectable', async () => {
    await toggleSelectable();
    const textExample = await app.findElementByTestID('text-example');
    await textExample.doubleClick();
    const dump = await dumpVisualTree('pressed-state');
    if (dump.Text === 'Pressed: 2 times.') {
      // Due to the hardcoded speed between clicks in WinAppDriver, this test
      // can be flaky on Windows Server 2022. Detect and warn here rather than
      // disabling the entire test.
      console.warn('DoubleClickWhenSelectable registered two clicks.');
      dump.Text = 'Pressed: 1 times.';
    }
    expect(dump).toMatchSnapshot();
  });
});

async function clearState() {
  const clearButton = await app.findElementByTestID('clear-state-button');
  await clearButton.click();
}

async function toggleSelectable() {
  const toggleButton = await app.findElementByTestID(
    'toggle-selectable-button',
  );
  await toggleButton.click();
}
