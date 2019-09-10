/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import TextInputTestPage from '../pages/TextInputTestPage';
import HomePage from '../pages/HomePage';
import assert from 'assert';

before(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGoToTextInputPage();
});

describe('First', () => {
  it('Type abc on TextInput', () => {
    TextInputTestPage.clearAndTypeOnTextInput('abc');
    assert.equal(TextInputTestPage.getTextInputText(), 'abc');
  });

  it('Type def on TextInput', () => {
    TextInputTestPage.clearAndTypeOnTextInput('def');
    assert.equal(TextInputTestPage.getTextInputText(), 'def');
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
