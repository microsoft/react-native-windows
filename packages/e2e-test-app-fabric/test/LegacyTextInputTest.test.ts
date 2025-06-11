/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import {app} from '@react-native-windows/automation';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';
import {dumpVisualTree} from '@react-native-windows/automation-commands';

beforeAll(async () => {
  // If window is partially offscreen, tests will fail to click on certain elements
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('LegacyTextInputTest');
});

afterEach(async () => {
  await verifyNoErrorLogs();
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
    await assertLogContains('onKeyPress key: c');
  });
  test('Type def on TextInput', async () => {
    const textInput = await textInputField();
    await app.waitUntil(
      async () => {
        await textInput.setValue('def');
        return (await textInput.getText()) === 'def';
      },
      {
        interval: 1500,
        timeout: 5000,
        timeoutMsg: `Unable to enter correct text.`,
      },
    );
    expect(await textInput.getText()).toBe('def');
  });
  /* Issue to enable these tests: #12778
  test('Type hello world on autoCap TextInput', async () => {
    const textInput = await autoCapsTextInputField();
    await textInput.setValue('def');
    expect(await textInput.getText()).toBe('DEF');

    await textInput.setValue('hello world');
    expect(await textInput.getText()).toBe('HELLO WORLD');
  });
  */
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
    await assertLogContains('onChange text: a\nonSelectionChange range: 1,1');
    /* Issue to enable these tests: #12778
    await assertLogContainsInOrder([
      'onChange text: a',
      'onSelectionChange range: 1,1',
    ]);
    */
  });
  
  // Additional functional tests for event handlers
  test('TextInput should trigger action upon onBlur', async () => {
    const textInput = await textInputField();
    await textInput.click(); // Focus
    
    const multiLineTextInput = await multiLineTextInputField();
    await multiLineTextInput.click(); // This should trigger onBlur on the first TextInput
    
    await assertLogContains('onBlur');
  });
  
  test('TextInput should trigger action upon onFocus', async () => {
    const textInput = await textInputField();
    await textInput.click(); // This should trigger onFocus
    
    await assertLogContains('onFocus');
  });
  
  test('TextInput should trigger action upon onChange', async () => {
    const textInput = await textInputField();
    await textInput.setValue('test');
    
    await assertLogContains('onChange text: test');
  });
  
  test('TextInput should trigger action upon onSelectionChange', async () => {
    const textInput = await textInputField();
    await textInput.setValue('test');
    
    await assertLogContains('onSelectionChange range: 4,4');
  });
});

async function textInputField() {
  const component = await app.findElementByTestID('textinput-field');
  await component.waitForDisplayed({timeout: 5000});
  return component;
}

/* Issue to enable these tests: #12778
async function autoCapsTextInputField() {
  const component = await app.findElementByTestID('auto-caps-textinput-field');
  await component.waitForDisplayed({timeout: 5000});
  return component;
}
*/

async function multiLineTextInputField() {
  const component = await app.findElementByTestID('multi-line-textinput-field');
  await component.waitForDisplayed({timeout: 5000});
  return component;
}

async function assertLogContains(_text: string) {
  const textLogComponent = await app.findElementByTestID('textinput-log');
  await textLogComponent.waitForDisplayed({timeout: 5000});

  const dump = await dumpVisualTree('textinput-log');
  expect(dump).toMatchSnapshot();
  /*
  await app.waitUntil(
    async () => {
      const loggedText = await textLogComponent.getText();
      return loggedText.split('\n').includes(text);
    },
    {
      timeoutMsg: `"${await textLogComponent.getValue()}" "${await textLogComponent.getText()}" did not contain "${text}"`,
    },
  );
  */
}

/*
async function assertLogContainsInOrder(expectedLines: string[]) {
  const textLogComponent = await app.findElementByTestID('textinput-log');
  await textLogComponent.waitForDisplayed({timeout: 5000});

  await app.waitUntil(
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
*/
