/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import DirectManipulationPage from '../pages/DirectManipulationPage';
import assert from 'assert';

beforeAll(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGotoDirectManipulationPage();
});

describe('DirectManipulationTest', () => {
  it('measureInWindow Success', () => {
    const result = DirectManipulationPage.clickMeasureInWindowAndGetResult();
    assert.ok(
      result.includes('width='),
      'measureInWindow response include width'
    );
    assert.ok(result.includes('x=0;'), 'measureInWindow response x=0');
  });
});
