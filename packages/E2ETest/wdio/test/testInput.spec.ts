/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import TextInputTestPage from '../pages/TextInputTestPage';
import HomePage from '../pages/HomePage';
import assert from 'assert';

beforeAll(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGoToTextInputPage();
});

describe('First', () => {
  it('Click on TextInput to focus', () => {
    TextInputTestPage.clickTextInput();
    assert.ok(TextInputTestPage.getTextInputCurText().includes('onFocus'));
  });

  it('Click on multiline TextInput to move focus away from single line TextInput', () => {
    TextInputTestPage.clickMultilineTextInput();
    assert.ok(TextInputTestPage.getTextInputPrevText().includes('onBlur'));
  });

  it('Type abc on TextInput', () => {
    TextInputTestPage.clearAndTypeOnTextInput('abc');
    assert.equal(TextInputTestPage.getTextInputText(), 'abc');

    // Due to some timing issues between the JS and native, the order of events
    // might cause more onChange events to happen after the onKeyPress event
    // So check the last few events for onKeyPress.
    // eslint-disable-next-line no-undef
    browser.waitUntil(
      () =>
        TextInputTestPage.getTextInputPrev2Text().includes(
          'onKeyPress key: c'
        ) ||
        TextInputTestPage.getTextInputPrev3Text().includes(
          'onKeyPress key: c'
        ) ||
        TextInputTestPage.getTextInputPrev4Text().includes('onKeyPress key: c'),
      1000,
      'Wait for text to be updated'
    );

    assert.ok(
      TextInputTestPage.getTextInputPrev2Text().includes('onKeyPress key: c') ||
        TextInputTestPage.getTextInputPrev3Text().includes(
          'onKeyPress key: c'
        ) ||
        TextInputTestPage.getTextInputPrev4Text().includes('onKeyPress key: c')
    );
  });

  it('Type def on TextInput', () => {
    TextInputTestPage.clearAndTypeOnTextInput('def');
    assert.equal(TextInputTestPage.getTextInputText(), 'def');
  });

  it('Type hello world on autoCap TextInput', () => {
    TextInputTestPage.clearAndTypeOnAutoCapTextInput('hello world');
    assert.equal(TextInputTestPage.getAutoCapTextInput(), 'HELLO WORLD');
  });

  it('Type abc on multiline TextInput then press Enter key', () => {
    TextInputTestPage.clearAndEnterOnTextInput('abc');
    assert.equal(
      TextInputTestPage.getTextInputPrevText(),
      'prev: onSubmitEditing text: abc'
    );
  });

  it('Type abc on multiline TextInput', () => {
    TextInputTestPage.clearAndTypeOnMLTextInput('abc');
    assert.equal(TextInputTestPage.getMLTextInputText(), 'abc');
  });

  it('Enter key then type def on multiline TextInput', () => {
    TextInputTestPage.appendNewLineOnMLText('def');
    assert.equal(TextInputTestPage.getMLTextInputText(), 'abc\rdef');
  });
});
