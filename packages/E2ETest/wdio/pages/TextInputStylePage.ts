/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import { SHOWBORDER_ON_TEXTINPUTSTYLE } from '../../app/Consts';

class TextInputStyleTestPage extends BasePage {
  backToHomePage() {
    this.homeButton.click();
    this.waitForPageLoaded();
  }

  isPageLoaded() {
    return super.isPageLoaded();
  }

  toggleTextInputBorder() {
    this._textInputBorder.click();
  }

  private get _textInputBorder() {
    return By(SHOWBORDER_ON_TEXTINPUTSTYLE);
  }
}

export default new TextInputStyleTestPage();
