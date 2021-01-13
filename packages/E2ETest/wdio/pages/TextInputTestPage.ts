/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { BasePage, by } from './BasePage';
import {
  TEXTINPUT_ON_TEXTINPUT,
  ML_TEXTINPUT_ON_TEXTINPUT,
  CAP_TEXTINPUT_ON_TEXTINPUT,
  CURTEXT_ON_TEXTINPUT,
} from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

class TextInputTestPage extends BasePage {
  async clickTextInput() {
    await (await this.textInput).click();
  }

  async clickMultilineTextInput() {
    await (await this.multiLineTextInput).click();
  }

  async clearAndTypeOnTextInput(text: string) {
    await (await this.textInput).setValue(text);
  }

  async clearAndEnterOnTextInput(text: string) {
    await (await this.textInput).setValue(text);
    await (await this.textInput).addValue('Enter');
  }

  async clearAndTypeOnMLTextInput(text: string) {
    await (await this.multiLineTextInput).setValue(text);
  }

  async clearAndTypeOnAutoCapTextInput(text: string) {
    await (await this.autoCapTextInput).setValue(text);
  }

  async appendNewLineOnMLText(text: string) {
    await (await this.multiLineTextInput).addValue('End');
    await (await this.multiLineTextInput).addValue('Enter');
    await (await this.multiLineTextInput).addValue(text);
  }

  async getTextInputCurText() {
    return (await this.curTextInput).getText();
  }

  async getTextInputText() {
    return (await this.textInput).getText();
  }

  async getMLTextInputText() {
    return (await this.multiLineTextInput).getText();
  }

  async getAutoCapTextInput() {
    return (await this.autoCapTextInput).getText();
  }

  private get textInput() {
    return by(TEXTINPUT_ON_TEXTINPUT);
  }

  private get curTextInput() {
    return by(CURTEXT_ON_TEXTINPUT);
  }

  private get multiLineTextInput() {
    return by(ML_TEXTINPUT_ON_TEXTINPUT);
  }

  private get autoCapTextInput() {
    return by(CAP_TEXTINPUT_ON_TEXTINPUT);
  }
}

export default new TextInputTestPage();
