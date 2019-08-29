/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import { TEXTINPUT_ON_TEXTINPUT } from '../../app/Consts';

class TextInputTestPage extends BasePage {
  isPageLoaded() {
    return super.isPageLoaded() && this.textInput.isDisplayed();
  }

  clearAndTypeOnTextInput(text: string) {
    this.textInput.setValue(text);
  }

  getTextInputText() {
    return this.textInput.getText();
  }

  private get textInput() {
    return By(TEXTINPUT_ON_TEXTINPUT);
  }
}

export default new TextInputTestPage();
