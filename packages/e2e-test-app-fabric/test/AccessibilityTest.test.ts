/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { dumpVisualTree } from '@react-native-windows/automation-commands';
import { goToApiExample } from './RNTesterNavigation';
import { app } from '@react-native-windows/automation';
import { verifyNoErrorLogs } from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToApiExample('Accessibility Windows');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

const searchBox = async (input: string) => {
  const searchBox = await app.findElementByTestID('example_search');
  await app.waitUntil(
    async () => {
      await searchBox.setValue(input);
      return (await searchBox.getText()) === input;
    },
    {
      interval: 1500,
      timeout: 5000,
      timeoutMsg: `Unable to enter correct search text into test searchbox.`,
    },
  );
};

// TODO: Re-enable once element accessibility issues are resolved
describe.skip('Accessibility Tests', () => {
  test('Elements can set accessibilityState:selected to false', async () => {
    await searchBox('Sta');
    const component = await app.findElementByTestID('Selectable item 1');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('Selectable item 1');
    expect(dump).toMatchSnapshot();
  });
  test('Elements can set accessibilityState:selected to true', async () => {
    await searchBox('Sta');
    const component = await app.findElementByTestID('Selectable item 1');
    await component.waitForDisplayed({ timeout: 5000 });
    await component.click();
    const dump = await dumpVisualTree('Selectable item 1');
    expect(dump).toMatchSnapshot();
  });
  test('Selectable items must have a Selection Container. Elements can set accessibilityState:multiselectable and accessibilityState:required to true', async () => {
    await searchBox('Sta');
    const componentsTab = await app.findElementByTestID('selection-container');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('selection-container');
    expect(dump).toMatchSnapshot();
  });
  test('Components can store range data by setting the min, max, and now of accessibilityValue', async () => {
    await searchBox('Sta');
    const componentsTab = await app.findElementByTestID(
      'accessibilityValue-number',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibilityValue-number');
    expect(dump).toMatchSnapshot();
  });
  test('Components can store value data by setting the text of accessibilityValue', async () => {
    await searchBox('Sta');
    const componentsTab = await app.findElementByTestID(
      'accessibilityValue-text',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibilityValue-text');
    expect(dump).toMatchSnapshot();
  });
  test('Accessibility data for Label,Level and Hint', async () => {
    await searchBox('Lab');
    const componentsTab = await app.findElementByTestID(
      'accessibility-base-view-1',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibility-base-view-1');
    expect(dump).toMatchSnapshot();
  });
  test('Accessibility data for Label and Level', async () => {
    await searchBox('Lab');
    const componentsTab = await app.findElementByTestID(
      'accessibility-base-view-2',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibility-base-view-2');
    expect(dump).toMatchSnapshot();
  });
  test('Accessibility data for Role, Setsize etc.', async () => {
    await searchBox('Lab');
    const componentsTab = await app.findElementByTestID(
      'accessibility-base-view-3',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibility-base-view-3');
    expect(dump).toMatchSnapshot();
  });
});
