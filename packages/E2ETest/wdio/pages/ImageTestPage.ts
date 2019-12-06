/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import { IMAGE_TREE_DUMP_RESULT } from '../../app/Consts';

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

  private get treeDumpResult() {
    return By(IMAGE_TREE_DUMP_RESULT);
  }
}

export default new ImageTestPage();
