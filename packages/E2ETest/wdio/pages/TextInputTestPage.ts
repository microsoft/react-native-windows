/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import {
  TEXTINPUT_ON_TEXTINPUT,
  ML_TEXTINPUT_ON_TEXTINPUT,
  CAP_TEXTINPUT_ON_TEXTINPUT,
  PREVTEXT_ON_TEXTINPUT,
} from '../../app/Consts';

class TextInputTestPage extends BasePage {
  isPageLoaded() {
    return super.isPageLoaded() && this.textInput.isDisplayed();
  }

  clearAndTypeOnTextInput(text: string) {
    this.textInput.setValue(text);
  }

  clearAndEnterOnTextInput(text: string) {
    this.textInput.setValue(text);
    this.textInput.addValue('Enter');
  }

  clearAndTypeOnMLTextInput(text: string) {
    this.multiLineTextInput.setValue(text);
  }

  clearAndTypeOnAutoCapTextInput(text: string) {
    this.autoCapTextInput.setValue(text);
  }

  appendNewLineOnMLText(text: string) {
    this.multiLineTextInput.addValue('End');
    this.multiLineTextInput.addValue('Enter');
    this.multiLineTextInput.addValue(text);
  }

  getTextInputPrevText() {
    return this.prevTextInput.getText();
  }

  getTextInputText() {
    return this.textInput.getText();
  }

  getMLTextInputText() {
    return this.multiLineTextInput.getText();
  }

  getAutoCapTextInput() {
    return this.autoCapTextInput.getText();
  }

  private get textInput() {
    return By(TEXTINPUT_ON_TEXTINPUT);
  }

  private get prevTextInput() {
    return By(PREVTEXT_ON_TEXTINPUT);
  }

  private get multiLineTextInput() {
    return By(ML_TEXTINPUT_ON_TEXTINPUT);
  }

  private get autoCapTextInput() {
    return By(CAP_TEXTINPUT_ON_TEXTINPUT);
  }
}

export default new TextInputTestPage();
