/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import TextInputTestPage from '../pages/TextInputTestPage';
import HomePage from '../pages/HomePage';
import { TEXTINPUT_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(() => {
  HomePage.goToComponentExample(TEXTINPUT_TESTPAGE);
});

function assertLogContains(text: string) {
  const log = TextInputTestPage.getTextInputCurText();
  expect(log).toBeTruthy();
  expect(log.split('\n').includes(text)).toBe(
    true,
    `${log} did not contain "${text}"`
  );
}

describe('First', () => {
  it('Click on TextInput to focus', () => {
    TextInputTestPage.clickTextInput();
    assertLogContains('onFocus');
  });

  it('Click on multiline TextInput to move focus away from single line TextInput', () => {
    TextInputTestPage.clickMultilineTextInput();
    assertLogContains('onBlur');
  });

  it('Type abc on TextInput', () => {
    TextInputTestPage.clearAndTypeOnTextInput('abc');
    expect(TextInputTestPage.getTextInputText()).toBe('abc');

    // Due to some timing issues between the JS and native, the order of events
    // might cause more onChange events to happen after the onKeyPress event
    // So the onKeyPress event might not be the last item in the log
    assertLogContains('onKeyPress key: c');
  });

  it('Type def on TextInput', () => {
    TextInputTestPage.clearAndTypeOnTextInput('def');
    expect(TextInputTestPage.getTextInputText()).toBe('def');
  });

  it('Type hello world on autoCap TextInput', () => {
    TextInputTestPage.clearAndTypeOnAutoCapTextInput('hello world');
    expect(TextInputTestPage.getAutoCapTextInput()).toBe('HELLO WORLD');
  });

  it('Type abc on multiline TextInput then press Enter key', () => {
    TextInputTestPage.clearAndEnterOnTextInput('abc');
    assertLogContains('onSubmitEditing text: abc');
  });

  it('Type abc on multiline TextInput', () => {
    TextInputTestPage.clearAndTypeOnMLTextInput('abc');
    expect(TextInputTestPage.getMLTextInputText()).toBe('abc');
  });

  it('Enter key then type def on multiline TextInput', () => {
    TextInputTestPage.appendNewLineOnMLText('def');
    expect(TextInputTestPage.getMLTextInputText()).toBe('abc\rdef');
  });
});
