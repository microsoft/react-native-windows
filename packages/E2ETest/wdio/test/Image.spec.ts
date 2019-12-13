/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import HomePage from '../pages/HomePage';
import ImageTestPage from '../pages/ImageTestPage';
import assert from 'assert';

beforeAll(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGotoImagePage();
});

describe('ImageWithoutBorderTest', () => {
  it('ImageWithoutBorderTest', () => {
    const result = ImageTestPage.getTreeDumpResult();
    assert(result, '#1. Dump comparison for image without border!');
  });

  it('ImageWithBorderTest', () => {
    ImageTestPage.toggleImageBorder();
    const result = ImageTestPage.getTreeDumpResult();
    assert(result, '#2. Dump comparison for image with border!');
  });

  // toggle back to no border and verify border properties are reset
  it('ImageWithoutBorderTest', () => {
    ImageTestPage.toggleImageBorder();
    const result = ImageTestPage.getTreeDumpResult();
    assert(result, '#3. Second dump comparison for image without border!');
  });
});
