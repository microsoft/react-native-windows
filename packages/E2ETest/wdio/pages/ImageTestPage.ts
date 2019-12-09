/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import { TREE_DUMP_RESULT, SHOW_IMAGE_BORDER } from '../../app/Consts';

class ImageTestPage extends BasePage {
  backToHomePage() {
    this.homeButton.click();
    this.waitForPageLoaded();
  }

  isPageLoaded() {
    return super.isPageLoaded() && this.treeDumpResult.isDisplayed();
  }

  getTreeDumpResult() {
    return this.treeDumpResult.getText();
  }

  toggleImageBorder() {
    this._imageBorder.click();
  }

  private get treeDumpResult() {
    return By(TREE_DUMP_RESULT);
  }

  private get _imageBorder() {
    return By(SHOW_IMAGE_BORDER);
  }
}

export default new ImageTestPage();
