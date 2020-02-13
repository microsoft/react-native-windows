/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import { SHOW_IMAGE_BORDER, SET_RTL_MODE } from '../../app/Consts';

class ImageTestPage extends BasePage {
  backToHomePage() {
    this.homeButton.click();
    this.waitForPageLoaded();
  }

  isPageLoaded() {
    return super.isPageLoaded();
  }

  toggleImageBorder() {
    this._imageBorder.click();
  }

  toggleRTLMode() {
    this._rtlButton.click();
  }

  private get _imageBorder() {
    return By(SHOW_IMAGE_BORDER);
  }

  private get _rtlButton() {
    return By(SET_RTL_MODE);
  }
}

export default new ImageTestPage();
