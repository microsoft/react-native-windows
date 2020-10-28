/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import DirectManipulationPage from '../pages/DirectManipulationPage';
import { DIRECT_MANIPULATION_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(() => {
  HomePage.goToComponentExample(DIRECT_MANIPULATION_TESTPAGE);
});

describe('DirectManipulationTest', () => {
  it('measureInWindow Success', () => {
    const result = DirectManipulationPage.clickMeasureInWindowAndGetResult();
    expect(result.includes('width=')).toBe(
      true,
      'measureInWindow response include width'
    );
  });

  it('measureLayout Test', () => {
    const result = DirectManipulationPage.clickMeasureLayoutAndGetResult();
    expect(result.includes('width=50')).toBe(
      true,
      'measureLayout response has correct width'
    );
    expect(result.includes('x=20')).toBe(true, 'measureLayout response x=20');
  });
});
