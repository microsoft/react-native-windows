/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToApiExample, goToComponentExample} from './RNTesterNavigation';

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

describe('visitAllPages', () => {
  for (const component of componentExamples) {
    test(component, async () => await goToComponentExample(component));
  }

  for (const api of apiExamples) {
    test(api, async () => await goToApiExample(api));
  }
});
