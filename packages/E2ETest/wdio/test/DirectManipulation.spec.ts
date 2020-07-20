/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import DirectManipulationPage from '../pages/DirectManipulationPage';
import assert from 'assert';

beforeAll(() => {
  HomePage.clickAndGotoDirectManipulationPage();
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
