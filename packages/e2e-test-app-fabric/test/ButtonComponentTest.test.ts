/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { dumpVisualTree } from '@react-native-windows/automation-commands';
import { goToComponentExample } from './RNTesterNavigation';
import { verifyNoErrorLogs } from './Helpers';
import { app } from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Button');
});

afterEach(async () => {
  await verifyNoErrorLogs();
  await searchBox('');
});

const searchBox = async (input: string) => {
  const searchBox = await app.findElementByTestID('example_search');
  await app.waitUntil(
    async () => {
      await searchBox.setValue(input);
      if (input === '') {
        return (await searchBox.getText()) === 'Search...';
      } else {
        return (await searchBox.getText()) === input;
      }
    },
    {
      interval: 1500,
      timeout: 5000,
      timeoutMsg: `Unable to enter correct search text into test searchbox.`,
    },
  );
};

// TODO: Re-enable once element accessibility issues are resolved
describe.skip('Button Tests', () => {
  test('Buttons have default styling', async () => {
    await searchBox('default');
    const component = await app.findElementByTestID('button_default_styling');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('button_default_styling');
    expect(dump).toMatchSnapshot();
    await component.click();
    const alert = await app.findElementByTestID('Popup Window');
    await alert.isDisplayed();
    const alertButton = await app.findElementByTestID('Button0');
    await alertButton.click();
  });
  test('Buttons can have custom colors', async () => {
    await searchBox('color');
    const component = await app.findElementByTestID('cancel_button');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('cancel_button');
    expect(dump).toMatchSnapshot();
    await component.click();
    const alert = await app.findElementByTestID('Popup Window');
    await alert.isDisplayed();
    const alertButton = await app.findElementByTestID('Button0');
    await alertButton.click();
  });
  test('Buttons can have flexbox styling', async () => {
    const component = await app.findElementByTestID('two_button_container');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('two_button_container');
    expect(dump).toMatchSnapshot();
  });
  test('Buttons can have flexbox styling with three buttons', async () => {
    const component = await app.findElementByTestID('three_button_container');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('three_button_container');
    expect(dump).toMatchSnapshot();
  });
  test('Buttons can be disabled', async () => {
    await searchBox('dis');
    const component = await app.findElementByTestID('disabled_button');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('disabled_button');
    expect(dump).toMatchSnapshot();
    const alert = await app.findElementByTestID('Popup Window');
    expect(await alert.isDisplayed()).toBe(false);
  });
  test('Buttons can have accessibility labels', async () => {
    await searchBox('label');
    const component = await app.findElementByTestID(
      'accessibilityLabel_button',
    );
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibilityLabel_button');
    expect(dump).toMatchSnapshot();
    await component.click();
    const alert = await app.findElementByTestID('Popup Window');
    await alert.isDisplayed();
    const alertButton = await app.findElementByTestID('Button0');
    await alertButton.click();
  });
  test('Buttons can have accessibility states', async () => {
    await searchBox('state');
    const component = await app.findElementByTestID(
      'accessibilityState_button',
    );
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibilityState_button');
    expect(dump).toMatchSnapshot();
    const alert = await app.findElementByTestID('Popup Window');
    expect(await alert.isDisplayed()).toBe(false);
  });
  test('Buttons can have accessibility props', async () => {
    await searchBox('props');
    const component = await app.findElementByTestID('accessibility_props');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibility_props');
    expect(dump).toMatchSnapshot();
    await component.click();
    const alert = await app.findElementByTestID('Popup Window');
    await alert.isDisplayed();
    const alertButton = await app.findElementByTestID('Button0');
    await alertButton.click();
  });
  test('Buttons can have custom focusable and accessible props', async () => {
    await searchBox('prop');
    const dump = await dumpVisualTree('accessible_false_button');
    expect(dump).toMatchSnapshot();
    const componentNotFocusable = await app.findElementByTestID(
      'focusable_false_button',
    );

    await componentNotFocusable.waitForDisplayed({ timeout: 5000 });
    const dump2 = await dumpVisualTree('focusable_false_button');
    expect(dump2).toMatchSnapshot();
    const dump3 = await dumpVisualTree('accessible_focusable_false_button');
    expect(dump3).toMatchSnapshot();
  });
});
