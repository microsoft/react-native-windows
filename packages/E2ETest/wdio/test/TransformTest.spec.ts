/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import TransformTestPage from '../pages/TransformTestPage';
import assert from 'assert';

beforeAll(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGotoTransformTestPage();
});

describe('TransformTest', () => {
  it('Measure original size and position', () => {
    const result = TransformTestPage.clickMeasureLayoutAndGetResult();
    assert.ok(result.includes('width=60'), 'measureLayout response width=60');
    assert.ok(result.includes('x=0;'), 'measureLayout response x=20');
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
