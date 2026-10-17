/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import {goToComponentExample} from './RNTesterNavigation';
import {verifyNoErrorLogs} from './Helpers';

beforeAll(async () => {
  await app.setWindowPosition(0, 0);
  await app.setWindowSize(1000, 1250);
  await goToComponentExample('Custom TextInput component Example');
});

afterEach(async () => {
  await verifyNoErrorLogs();
});

describe('CustomTextInput', () => {
  test('Maps initial value and defaultValue to native text', async () => {
    const controlled = await app.findElementByTestID('custom-textinput-1');
    const uncontrolled = await app.findElementByTestID(
      'custom-textinput-default',
    );
    expect(await controlled.getText()).toBe('hello');
    expect(await uncontrolled.getText()).toBe('default value');
  });

  test('Adapts both change callbacks and synchronizes controlled edits', async () => {
    const input = await app.findElementByTestID('custom-textinput-1');
    await input.setValue('world');
    await app.waitUntil(async () => (await input.getText()) === 'WORLD', {
      timeoutMsg:
        'The controlled value should reflect onChangeText transformations.',
    });
    const changeLog = await app.findElementByTestID('custom-textinput-change');
    expect((await changeLog.getText()).toUpperCase()).toBe('WORLD');
  });

  test('Keeps uncontrolled edits when onChangeText triggers a render', async () => {
    const input = await app.findElementByTestID('custom-textinput-default');
    await input.setValue('changed');
    const changeLog = await app.findElementByTestID(
      'custom-textinput-default-change',
    );
    await app.waitUntil(async () => (await changeLog.getText()) === 'changed', {
      timeoutMsg: 'Uncontrolled inputs should invoke onChangeText.',
    });
    expect(await input.getText()).toBe('changed');
  });

  test('Restores a controlled value when JavaScript rejects an edit', async () => {
    const input = await app.findElementByTestID('custom-textinput-fixed');
    await input.setValue('ignored');
    await app.waitUntil(async () => (await input.getText()) === 'fixed', {
      timeoutMsg: 'Rejected native edits should restore the controlled value.',
    });
  });
});
