/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {goToComponentExample} from './framework';

beforeAll(async () => {
  await goToComponentExample('LegacyTextInputTest');
});

describe('LegacyTextInputTest', () => {
  test('Click on TextInput to focus', async () => {
    const textInput = await textInputField();
    await textInput.click();
    await assertLogContains('onFocus');
  });

  test('Click on multiline TextInput to move focus away from single line TextInput', async () => {
    const textInput = await multiLineTextInputField();
    await textInput.click();
    await assertLogContains('onBlur');
  });

  test('Type abc on TextInput', async () => {
    const textInput = await textInputField();
    await textInput.setValue('abc');
    expect(await textInput.getText()).toBe('abc');

    // Due to some timing issues between the JS and native, the order of events
    // might cause more onChange events to happen after the onKeyPress event
    // So the onKeyPress event might not be the last item in the log
    await assertLogContains('onKeyPress key: c');
  });

  test('Type def on TextInput', async () => {
    const textInput = await textInputField();
    await textInput.setValue('def');
    expect(await textInput.getText()).toBe('def');
  });

  test('Type hello world on autoCap TextInput', async () => {
    const textInput = await autoCapsTextInputField();
    await textInput.setValue('def');
    expect(await textInput.getText()).toBe('DEF');

    await textInput.setValue('hello world');
    expect(await textInput.getText()).toBe('HELLO WORLD');
  });

  test('Type abc on multiline TextInput then press Enter key', async () => {
    const textInput = await textInputField();
    await textInput.setValue('abc');
    await textInput.addValue('Enter');

    await assertLogContains('onSubmitEditing text: abc');
  });

  test('Type abc on multiline TextInput', async () => {
    const textInput = await multiLineTextInputField();
    await textInput.setValue('abc');

    expect(await textInput.getText()).toBe('abc');
  });

  test('Enter key then type def on multiline TextInput', async () => {
    const textInput = await multiLineTextInputField();

    await textInput.addValue('End');
    await textInput.addValue('Enter');
    await textInput.addValue('def');

    expect(await textInput.getText()).toBe('abc\rdef');
  });

  test('TextInput onChange before onSelectionChange', async () => {
    const textInput = await textInputField();
    await textInput.setValue('a');
    await assertLogContainsInOrder(
      'onChange text: a',
      'onSelectionChange range: 1,1',
    );
  });
});

async function textInputField() {
  return await $('~textinput-field');
}

async function autoCapsTextInputField() {
  return await $('~auto-caps-textinput-field');
}

async function multiLineTextInputField() {
  return await $('~multi-line-textinput-field');
}

async function assertLogContains(text: string) {
  const textLogComponent = await $('~textinput-log');

  await browser.waitUntil(
    async () => {
      const loggedText = await textLogComponent.getText();
      return loggedText.split('\n').includes(text);
    },
    {
      timeoutMsg: `"${await textLogComponent.getText()}" did not contain "${text}"`,
    },
  );
}

async function assertLogContainsInOrder(...expectedLines: string[]) {
  const textLogComponent = await $('~textinput-log');

  await browser.waitUntil(
    async () => {
      const loggedText = await textLogComponent.getText();
      const actualLines = loggedText.split('\n');
      let previousIndex = Number.MAX_VALUE;
      for (const line of expectedLines) {
        const index = actualLines.findIndex(l => l === line);
        if (index === -1 || index > previousIndex) {
          return false;
        }

        previousIndex = index;
      }

      return true;
    },
    {
      timeoutMsg: `"${await textLogComponent.getText()}" did not contain lines "${expectedLines.join(
        ', ',
      )}"`,
    },
  );
}
