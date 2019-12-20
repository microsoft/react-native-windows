/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import ControlStyleTestPage from '../pages/ControlStylePage';
import assert from 'assert';

beforeAll(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGotoControlStylePage();
});

describe('ControlStyleTest', () => {
  /* Test case #1: Controls style with regular border */
  it('ControlStyleTestWithRegularBorder', () => {
    const result = ControlStyleTestPage.getTreeDumpResult();
    assert(
      result,
      '#1. Dump comparison for Control style with regular border!'
    );
  });

  /* Test case #2: Click button once, update controls style and round border*/
  it('ControlStyleTestWithRoundBorder', () => {
    ControlStyleTestPage.toggleControlBorder();
    const result = ControlStyleTestPage.getTreeDumpResult();
    assert(result, '#2. Dump comparison for Control style with round border!');
  });

  /* Test case #3: Click button one more, return to #1*/
  it('ControlStyleTestWithRegularBorder #2', () => {
    ControlStyleTestPage.toggleControlBorder();
    const result = ControlStyleTestPage.getTreeDumpResult();
    assert(
      result,
      '#3. Second dump comparison for Control style with regular border!'
    );
  });
});
