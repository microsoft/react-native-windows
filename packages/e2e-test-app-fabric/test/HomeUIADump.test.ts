/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import { app } from '@react-native-windows/automation';
import { verifyNoErrorLogs } from './Helpers';
import { dumpVisualTree } from '@react-native-windows/automation-commands';

type RNTesterExampleModule = {
  title: string;
  description: string;
};

type RNTesterModuleInfo = {
  key: string;
  module: RNTesterExampleModule;
};

type RNTesterList = {
  APIs: RNTesterModuleInfo[];
  Components: RNTesterModuleInfo[];
};

const testerList: RNTesterList = require('@react-native-windows/tester/js/utils/RNTesterList');

const apiExamples = testerList.APIs.map(e => e.module.title);
const componentExamples = testerList.Components.map(e => e.module.title);

afterEach(async () => {
  await verifyNoErrorLogs();
});

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
});

// TODO: Re-enable once https://github.com/microsoft/react-native-windows/issues/15497 is fixed
describe.skip('Home UIA Tree Dump', () => {
  test('Components Tab', async () => {
    const componentsTabButton = await app.findElementByTestID('components-tab');
    await componentsTabButton.waitForDisplayed({ timeout: 20000 });
    const dump = await dumpVisualTree('components-tab');
    expect(dump).toMatchSnapshot();
  });
  test('APIs Tab', async () => {
    const apisTabButton = await app.findElementByTestID('apis-tab');
    await apisTabButton.waitForDisplayed({ timeout: 20000 });
    const dump = await dumpVisualTree('apis-tab');
    expect(dump).toMatchSnapshot();
  });
  test('Search Bar', async () => {
    const component = await app.findElementByTestID('explorer_search');
    await component.waitForDisplayed({ timeout: 20000 });
    const dump = await dumpVisualTree('explorer_search');
    expect(dump).toMatchSnapshot();
  });
  for (const component of componentExamples) {
    test(component, async () => {
      const componentsTabButton = await app.findElementByTestID(
        'components-tab',
      );
      await componentsTabButton.click();
      const componentTile = await app.findElementByTestID(component);
      await componentTile.waitForDisplayed({ timeout: 20000 });
      const dump = await dumpVisualTree(component);
      expect(dump).toMatchSnapshot();
    });
  }
  for (const api of apiExamples) {
    test(api, async () => {
      const apisTabButton = await app.findElementByTestID('apis-tab');
      await apisTabButton.click();
      const apiTile = await app.findElementByTestID(api);
      await apiTile.waitForDisplayed({ timeout: 20000 });
      const dump = await dumpVisualTree(api);
      expect(dump).toMatchSnapshot();
    });
  }
});

export { };
