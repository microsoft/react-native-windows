/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import {goToComponentExample} from './RNTesterNavigation';
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

  // Disable temporarily until tests become stable. Currently, windows loses focus randomly resulting in tests being stalled.
  /*for (const api of apiExamples) {
    if (
      api === 'Transforms' ||
      api === 'Keyboard Focus Example' ||
      api === 'Alerts' ||
      api === 'Linking' ||
      api === 'AppState' ||
      api === 'Border' ||
      api === 'Crash' ||
      api === 'Accessibility Windows' ||
      api === 'Accessibility'
    )
      // disable until either transformExample uses units, or that isn't an error
      continue;

    test(api, async () => {
      await goToApiExample(api);
    });
  }*/
});

export {};
