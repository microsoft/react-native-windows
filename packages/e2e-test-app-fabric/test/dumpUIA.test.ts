/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import {verifyNoErrorLogs} from './Helpers';
import {dumpVisualTree} from '@react-native-windows/automation-commands';

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

describe('Home Page UIA Tree Dump', () => {
  test('Components Tab UIA Dump', async () => {
    const componentsTabButton = await app.findElementByTestID('components-tab');
    await componentsTabButton.waitForDisplayed({timeout: 20000});
    const dump = await dumpVisualTree('components-tab');
    expect(dump).toMatchSnapshot();
  });
  test('APIs Tab UIA Dump', async () => {
    const dump = await dumpVisualTree('apis-tab');
    expect(dump).toMatchSnapshot();
  });
  test('Search Bar UIA Dump', async () => {
    const dump = await dumpVisualTree('explorer_search');
    expect(dump).toMatchSnapshot();
  });
  for (const component of componentExamples) {
    test(component, async () => {
      const componentsTabButton = await app.findElementByTestID(
        'components-tab',
      );
      await componentsTabButton.click();
      const dump = await dumpVisualTree(component);
      expect(dump).toMatchSnapshot();
    });
  }
  for (const api of apiExamples) {
    test(api, async () => {
      const apisTabButton = await app.findElementByTestID('apis-tab');
      await apisTabButton.click();
      const dump = await dumpVisualTree(api);
      expect(dump).toMatchSnapshot();
    });
  }
});

export {};
