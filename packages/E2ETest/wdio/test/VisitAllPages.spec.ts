/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import HomePage from '../pages/HomePage';

import {
  ApiExamples,
  ComponentExamples,
  // @ts-ignore no typings from internals in flow
} from '@react-native-windows/tester/js/utils/RNTesterList';
import { By } from '../pages/BasePage';

const ignoredExamples = [
  'Custom Views',
  'FlatList - MultiColumn',
  'SectionList',
  'Touchable* and onPress',
  'LegacyControlStyleTest',
  'LegacyTransformTest',
  'LegacyTextInputTest',
  'LegacyLoginTest',
  'LegacyDirectManipulationTest',
  'LegacyImageTest',
  'LegacyAccessibilityTest',
];

describe('VisitAllPagesTest', () => {
  ApiExamples.forEach((example: { key: string; module: any }) => {
    addVisitPageTest(example.key, 'api');
  });

  ComponentExamples.forEach((example: { key: string; module: any }) => {
    addVisitPageTest(example.key, 'component');
  });
});

function addVisitPageTest(name: string, type: 'api' | 'component') {
  if (!ignoredExamples.includes(name)) {
    it(name, () => {
      console.log('loading page ' + name);

      if (type === 'api') {
        HomePage.waitForElementLoaded('apis-tab');
        By('apis-tab').click();
      } else {
        HomePage.waitForElementLoaded('components-tab');
        By('components-tab').click();
      }

      HomePage.goToTestPage(name);
      HomePage.backToHomePage();
    });
  }
}
