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
  it('ImageWithoutBorderTest Success', () => {
    const result = ImageTestPage.getTreeDumpResult();
    assert.ok(
      result.includes('TreeDump:Passed'),
      'Dump comparison passed for image without border!'
    );
  });
});
