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

    // We should really develop a proper mechanism of waiting for idle, or detecting when the JS + bridge has had a chance of completing
    // But this test is causing a large itermittent failure rate on CI, so adding this polling timeout for now.
    // eslint-disable-next-line no-undef
    browser.waitUntil(
      () => TextInputTestPage.getTextInputPrev2Text().includes('onKeyPress'),
      1000,
      'Wait for text to be updated'
    );

    assert.ok(TextInputTestPage.getTextInputPrev2Text().includes('onKeyPress'));
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
