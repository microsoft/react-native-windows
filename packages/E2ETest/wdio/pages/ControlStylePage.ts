/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import { SHOWBORDER_ON_CONTROLSTYLE } from '../../app/Consts';

class ControlStyleTestPage extends BasePage {
  backToHomePage() {
    this.homeButton.click();
    this.waitForPageLoaded();
  }

  isPageLoaded() {
    return super.isPageLoaded();
  }

  toggleControlBorder() {
    this._controlBorder.click();
  }

  private get _controlBorder() {
    return By(SHOWBORDER_ON_CONTROLSTYLE);
  }
}

export default new ControlStyleTestPage();
