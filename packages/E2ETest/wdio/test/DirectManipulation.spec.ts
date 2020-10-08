/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import DirectManipulationPage from '../pages/DirectManipulationPage';
import assert from 'assert';
import { DIRECT_MANIPULATION_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(() => {
  HomePage.goToTestPage(DIRECT_MANIPULATION_TESTPAGE);
});

describe('DirectManipulationTest', () => {
  it('measureInWindow Success', () => {
    const result = DirectManipulationPage.clickMeasureInWindowAndGetResult();
    assert.ok(
      result.includes('width='),
      'measureInWindow response include width'
    );
  });

  it('measureLayout Test', () => {
    const result = DirectManipulationPage.clickMeasureLayoutAndGetResult();
    assert.ok(
      result.includes('width=50'),
      'measureLayout response has correct width'
    );
    assert.ok(result.includes('x=20;'), 'measureLayout response x=20');
  });
});
