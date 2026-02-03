/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';
import {app} from '@react-native-windows/automation';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

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

//const apiExamples = testerList.APIs.map(e => e.module.title);
const componentExamples = testerList.Components.map(e => e.module.title);

describe('visitAllPages', () => {
  test('control', () => {
    expect(true).toBe(true);
  });

  for (const component of componentExamples) {
    if (
      component === 'Flyout' ||
      component === 'XAML' ||
      component === 'Xaml WinUI3 (Experimental, for Fabric)' ||
      component === 'SwipeableCard'
    ) {
      console.log('Skipping: ' + component);
      continue;
    }

    console.log('Testing: ' + component);
    test(component, async () => await goToComponentExample(component));
  }

  // Disable Temporarily Until Stable
  /*for (const api of apiExamples) {
    if (api === 'Transforms' || api === 'Keyboard Focus Example')
      // Disable until tests are supported.
      continue;

    test(api, async () => {
      await goToApiExample(api);
    });
  }*/
});

export {};
