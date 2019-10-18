/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import TextInputTestPage from './TextInputTestPage';
import {
  TEXTINPUT_TESTPAGE,
  LOGIN_TESTPAGE,
  DIRECT_MANIPULATION_TESTPAGE,
} from '../../app/Consts';
import LoginPage from './LoginPage';
import DirectManipulationPage from './DirectManipulationPage';

class HomePage extends BasePage {
  backToHomePage() {
    this.homeButton.click();
    this.waitForPageLoaded();
  }

  isPageLoaded() {
    return super.isPageLoaded() && this.testInputTestPageButton.isDisplayed();
  }

  clickAndGoToTextInputPage() {
    this.testInputTestPageButton.click();
    TextInputTestPage.waitForPageLoaded();
  }

  clickAndGotoLoginPage() {
    this.loginTestPageButton.click();
    LoginPage.waitForPageLoaded();
  }

  clickAndGotoDirectManipulationPage() {
    this.directManipulationPageButton.click();
    DirectManipulationPage.waitForPageLoaded();
  }

  private get testInputTestPageButton() {
    return By(TEXTINPUT_TESTPAGE);
  }

  private get loginTestPageButton() {
    return By(LOGIN_TESTPAGE);
  }

  private get directManipulationPageButton() {
    return By(DIRECT_MANIPULATION_TESTPAGE);
  }
}

export default new HomePage();
