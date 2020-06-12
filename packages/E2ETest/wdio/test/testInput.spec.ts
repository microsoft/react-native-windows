/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import TextInputTestPage from '../pages/TextInputTestPage';
import HomePage from '../pages/HomePage';
import assert from 'assert';

beforeAll(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGoToTextInputPage();
});

function assertLogContains(text: string) {
  const log = TextInputTestPage.getTextInputCurText();
  assert.ok(log, `${log} should not be falsy`);
  assert.ok(log.split('\n').includes(text), `${log} did not contain "${text}"`);
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
    assert.equal(TextInputTestPage.getTextInputText(), 'abc');

    // Due to some timing issues between the JS and native, the order of events
    // might cause more onChange events to happen after the onKeyPress event
    // So the onKeyPress event might not be the last item in the log
    assertLogContains('onKeyPress key: c');
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
    assertLogContains('onSubmitEditing text: abc');
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
