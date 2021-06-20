/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample, goToApiExample} from './framework';

type RNTesterExampleModule = {
  title: string;
  testTitle?: string | null;
  description: string;
  displayName?: string | null;
};

type RNTesterExample = {
  key: string;
  module: RNTesterExampleModule;
  category?: string;
  exampleType?: 'components' | 'apis';
};

type RNTesterList = {
  APIExamples: RNTesterExample[];
  ComponentExamples: RNTesterExample[];
};

const testerList: RNTesterList = require('@react-native-windows/tester/js/utils/RNTesterList');

const apiExamples = testerList.APIExamples.map(e => e.module.title);
const componentExamples = testerList.ComponentExamples.map(e => e.module.title);

describe('visitAllPages', () => {
  for (const component of componentExamples) {
    test(component, async () => await goToComponentExample(component));
  }

  for (const api of apiExamples) {
    test(api, async () => await goToApiExample(api));
  }
});
