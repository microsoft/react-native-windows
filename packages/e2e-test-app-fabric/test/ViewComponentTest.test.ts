/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { dumpVisualTree } from '@react-native-windows/automation-commands';
import { goToComponentExample } from './RNTesterNavigation';
import { app } from '@react-native-windows/automation';
import { verifyNoErrorLogs } from './Helpers';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('View');
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

// TODO: Re-enable once https://github.com/microsoft/react-native-windows/issues/15497 is fixed
describe.skip('View Tests', () => {
  test('Views can have shadows', async () => {
    await searchBox('sha');
    const component = await app.findElementByTestID('shadow');
    await component.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('shadow');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have border styles', async () => {
    await searchBox('sty');
    const componentsTab = await app.findElementByTestID('border-style-button');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('border-style-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have offscreen alpha compositing', async () => {
    await searchBox('off');
    const componentsTab = await app.findElementByTestID(
      'offscreen-alpha-compositing-button',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('offscreen-alpha-compositing-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have a z-index', async () => {
    await searchBox('z');
    const componentsTab = await app.findElementByTestID('z-index-button');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('z-index-button');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have display: none', async () => {
    await searchBox('dis');
    const componentsTab = await app.findElementByTestID('display-none-button');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('display-none-button');
    expect(dump).toMatchSnapshot();
  });
  // NEW CODE
  test('Views can have background color', async () => {
    await searchBox('back');
    const componentsTab = await app.findElementByTestID(
      'view-test-background-color',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-background-color');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have borders', async () => {
    await searchBox('bor');
    const componentsTab = await app.findElementByTestID('view-test-border');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-border');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have customized pasdding and margins', async () => {
    await searchBox('pad');
    const componentsTab = await app.findElementByTestID(
      'view-test-padding-margin',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-padding-margin');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have rounded borders', async () => {
    await searchBox('bor');
    const componentsTab = await app.findElementByTestID(
      'view-test-rounded-borders',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-rounded-borders');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have overflow', async () => {
    await searchBox('ove');
    const componentsTab = await app.findElementByTestID('view-test-overflow');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-overflow');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have rounded borders', async () => {
    await searchBox('bor');
    const componentsTab = await app.findElementByTestID(
      'view-test-rounded-borders',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-rounded-borders');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have customized opacity', async () => {
    await searchBox('opa');
    const componentsTab = await app.findElementByTestID('view-test-opacity');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-opacity');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have tooltips', async () => {
    await searchBox('too');
    const componentsTab = await app.findElementByTestID('tool-tip');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('tool-tip');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have backface visibility', async () => {
    await searchBox('bac');
    const componentsTab = await app.findElementByTestID(
      'view-test-backface-visibility',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-backface-visibility');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have aria-labels', async () => {
    await searchBox('ari');
    const componentsTab = await app.findElementByTestID('view-test-aria-label');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-aria-label');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have flexgap', async () => {
    await searchBox('fle');
    const componentsTab = await app.findElementByTestID('view-test-flexgap');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-flexgap');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have insets', async () => {
    await searchBox('ins');
    const componentsTab = await app.findElementByTestID('view-test-insets');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-insets');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have customized accessibility', async () => {
    await searchBox('acc');
    const componentsTab = await app.findElementByTestID('accessibility');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('accessibility');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have a hitslop region', async () => {
    await searchBox('hit');
    const componentsTab = await app.findElementByTestID('hitslop');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('hitslop');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have logical border colors', async () => {
    await searchBox('log');
    const componentsTab = await app.findElementByTestID(
      'view-test-logical-border-color',
    );
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('view-test-logical-border-color');
    expect(dump).toMatchSnapshot();
  });
  test('Views can have a nativeid', async () => {
    await searchBox('nat');
    const componentsTab = await app.findElementByTestID('nativeid');
    await componentsTab.waitForDisplayed({ timeout: 5000 });
    const dump = await dumpVisualTree('nativeid');
    expect(dump).toMatchSnapshot();
  });
});
