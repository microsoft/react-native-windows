/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import TextInputStyleTestPage from '../pages/TextInputStylePage';
import assert from 'assert';

beforeAll(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGotoTextInputStylePage();
});

describe('TextInputStyleTest', () => {
  /* Test case #1: view and image displayed with no border and cornerRadius */
  it('TextInputStyleTestWithoutBorder', () => {
    const result = TextInputStyleTestPage.getTreeDumpResult();
    assert(result, '#1. Dump comparison for TextInput without border!');
  });

  /* Test case #2: Click button once, update view and image with round border*/
  it('TextInputStyleTestWithBorder', () => {
    TextInputStyleTestPage.toggleTextInputBorder();
    const result = TextInputStyleTestPage.getTreeDumpResult();
    assert(result, '#2. Dump comparison for TextInput with border!');
  });

  /* Test case #3: Click button one more, remove border from view and image but tree sturcture is different from #1*/
  it('TextInputStyleTestWithoutBorder', () => {
    TextInputStyleTestPage.toggleTextInputBorder();
    const result = TextInputStyleTestPage.getTreeDumpResult();
    assert(result, '#3. Second dump comparison for TextInput without border!');
  });
});
