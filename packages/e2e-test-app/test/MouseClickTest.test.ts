/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import {goToApiExample} from './RNTesterNavigation';

beforeAll(async () => {
  await goToApiExample('Mouse Click Events');
});

describe('Mouse Click Events', () => {
  beforeEach(async () => {
    await clearState();
  });

  test('Primary-Click on View', async () => {
    const view = await app.findElementByTestID('view_click');
    await view.click();
    void (await checkConsole(1, 0, 0));
  });

  test('Auxiliary-Click on View', async () => {
    const view = await app.findElementByTestID('view_click');
    await view.click({button: 1});
    void (await checkConsole(0, 1, 0));
  });

  test('Secondary-Click on View', async () => {
    const view = await app.findElementByTestID('view_click');
    await view.click({button: 2});
    void (await checkConsole(0, 0, 1));
  });

  test('Primary-Click on Button', async () => {
    const button = await app.findElementByTestID('button_click');
    await button.click();
    void (await checkConsole(1, 0, 0));
  });

  test('Auxiliary-Click on Button should not work', async () => {
    const button = await app.findElementByTestID('button_click');
    await button.click({button: 1});
    void (await checkConsole(0, 0, 0));
  });

  test('Secondary-Click on Button should not work', async () => {
    const button = await app.findElementByTestID('button_click');
    await button.click({button: 2});
    void (await checkConsole(0, 0, 0));
  });
});

async function clearState() {
  const clearButton = await app.findElementByTestID('clear_state_button');
  await clearButton.click();
}

async function checkConsole(
  expectedPrimary: int,
  expectedAuxiliary: int,
  expectedSecondary: int,
) {
  const textPrimary = await app.findElementByTestID('press_console_primary');
  const textAuxiliary = await app.findElementByTestID(
    'press_console_auxiliary',
  );
  const textSecondary = await app.findElementByTestID(
    'press_console_secondary',
  );
  expect(await textPrimary.getText()).toBe(
    'Primary Pressed x' + expectedPrimary,
  );
  expect(await textAuxiliary.getText()).toBe(
    'Auxiliary Pressed x' + expectedAuxiliary,
  );
  expect(await textSecondary.getText()).toBe(
    'Secondary Pressed x' + expectedSecondary,
  );
}
