/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import TextInputTestPage from '../pages/TextInputTestPage';
import HomePage from '../pages/HomePage';
import { TEXTINPUT_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(async () => {
  await HomePage.goToComponentExample(TEXTINPUT_TESTPAGE);
});

async function assertLogContains(text: string) {
  const log = await TextInputTestPage.getTextInputCurText();
  expect(log).toBeTruthy();
  expect(log.split('\n').includes(text)).toBe(
    true,
    `${log} did not contain "${text}"`
  );
}

describe('First', async () => {
  it('Click on TextInput to focus', async () => {
    await TextInputTestPage.clickTextInput();
    await assertLogContains('onFocus');
  });

  it('Click on multiline TextInput to move focus away from single line TextInput', async () => {
    await TextInputTestPage.clickMultilineTextInput();
    await assertLogContains('onBlur');
  });

  it('Type abc on TextInput', async () => {
    await TextInputTestPage.clearAndTypeOnTextInput('abc');
    expect(await TextInputTestPage.getTextInputText()).toBe('abc');

    // Due to some timing issues between the JS and native, the order of events
    // might cause more onChange events to happen after the onKeyPress event
    // So the onKeyPress event might not be the last item in the log
    await assertLogContains('onKeyPress key: c');
  });

  it('Type def on TextInput', async () => {
    await TextInputTestPage.clearAndTypeOnTextInput('def');
    expect(await TextInputTestPage.getTextInputText()).toBe('def');
  });

  it('Type hello world on autoCap TextInput', async () => {
    await TextInputTestPage.clearAndTypeOnAutoCapTextInput('hello world');
    expect(await TextInputTestPage.getAutoCapTextInput()).toBe('HELLO WORLD');
  });

  it('Type abc on multiline TextInput then press Enter key', async () => {
    await TextInputTestPage.clearAndEnterOnTextInput('abc');
    await assertLogContains('onSubmitEditing text: abc');
  });

  it('Type abc on multiline TextInput', async () => {
    await TextInputTestPage.clearAndTypeOnMLTextInput('abc');
    expect(await TextInputTestPage.getMLTextInputText()).toBe('abc');
  });

  it('Enter key then type def on multiline TextInput', async () => {
    await TextInputTestPage.appendNewLineOnMLText('def');
    expect(await TextInputTestPage.getMLTextInputText()).toBe('abc\rdef');
  });
});
