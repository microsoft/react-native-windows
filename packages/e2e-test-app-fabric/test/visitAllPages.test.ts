/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import {goToApiExample, goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

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
    if (component === 'Flyout' || component === 'XAML') {
      continue;
    }

    test(component, async () => await goToComponentExample(component));
  }

  for (const api of apiExamples) {
    if (api === 'Transforms' || api === 'Keyboard Focus Example')
      // Disable until tests are supported.
      continue;

    test(api, async () => {
      await goToApiExample(api);
    });
  }
});

export {};
