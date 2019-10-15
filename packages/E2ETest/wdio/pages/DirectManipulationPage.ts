/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import {
  MEASURE_IN_WINDOW_BUTTON,
  MEASURE_IN_WINDOW_RESULT,
} from '../../app/Consts';

class DirectManipulationPage extends BasePage {
  backToHomePage() {
    this.homeButton.click();
    this.waitForPageLoaded();
  }

  isPageLoaded() {
    return super.isPageLoaded() && this.measureInWindowButton.isDisplayed();
  }

  clickMeasureInWindowAndGetResult() {
    this.measureInWindowButton.click();
    return this.measureInWindowResult.getText();
  }

  private get measureInWindowButton() {
    return By(MEASURE_IN_WINDOW_BUTTON);
  }

  private get measureInWindowResult() {
    return By(MEASURE_IN_WINDOW_RESULT);
  }
}

export default new DirectManipulationPage();
