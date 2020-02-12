/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import {
  MEASURE_IN_WINDOW_BUTTON,
  DIRECT_MANIPULATION_RESULT,
  MEASURE_LAYOUT_BUTTON,
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
    return this.directManipulationResult.getText();
  }

  clickMeasureLayoutAndGetResult() {
    this.measureLayoutButton.click();
    return this.directManipulationResult.getText();
  }

  private get measureInWindowButton() {
    return By(MEASURE_IN_WINDOW_BUTTON);
  }

  private get measureLayoutButton() {
    return By(MEASURE_LAYOUT_BUTTON);
  }

  private get directManipulationResult() {
    return By(DIRECT_MANIPULATION_RESULT);
  }
}

export default new DirectManipulationPage();
