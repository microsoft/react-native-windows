/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import TransformTestPage from '../pages/TransformTestPage';
import { TRANSFORM_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(() => {
  HomePage.goToComponentExample(TRANSFORM_TESTPAGE);
});

describe('TransformTest', () => {
  it('Measure original size and position', () => {
    const result = TransformTestPage.clickMeasureLayoutAndGetResult();
    expect(result.includes('width=60')).toBe(
      true,
      'measureLayout response width=60'
    );
    expect(result.includes('x=0;')).toBe(true, 'measureLayout response x=20');
  });

  // Uncomment and run this test on 19h1+ when Platform.Version is available.
  // https://github.com/microsoft/react-native-windows/issues/4176

  // it('Apply 0.5 scale transform', () => {
  //   TransformTestPage.clickApplyScaleTransform();
  //   const result = TransformTestPage.clickMeasureLayoutAndGetResult();
  //   assert.ok(result.includes('width=30'), 'measureLayout response width=30');
  //   assert.ok(result.includes('x=15;'), 'measureLayout response x=15');
  // });
});
