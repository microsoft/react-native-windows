/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {app} from '@react-native-windows/automation';
import {goToComponentExample} from './RNTesterNavigation';

beforeAll(async () => {
  await goToComponentExample('LegacyLoginTest');
});

describe('LegacyLoginTest', () => {
  test('Login Success', async () => {
    await setUsername('username');
    await setPassword('password');

    await submitForm();
    expect(await getLoginResult()).toBe('Success');
  });

  test('Login Fail due to user email', async () => {
    await setUsername('username@microsoft.com');
    await setPassword('password');

    await submitForm();
    expect(await getLoginResult()).toBe('Fail');
  });

  test('Login Fail due to wrong password', async () => {
    await setUsername('username');
    await setPassword('abcdefg');

    await submitForm();
    expect(await getLoginResult()).toBe('Fail');
  });

  test('Login Success with secureTextEntry off', async () => {
    await toggleShowPassword();
    await setUsername('username');
    await setPassword('password');

    await submitForm();
    expect(await getLoginResult()).toBe('Success');
  });

  test('Login Success with secureTextEntry off then on', async () => {
    await setUsername('username');
    await setPassword('pass');

    await toggleShowPassword();
    await appendPassword('word');
    await submitForm();
    expect(await getLoginResult()).toBe('Success');
  });

  test('Login Success with secureTextEntry on then off', async () => {
    await setUsername('username');
    await setPassword('pass');

    await toggleShowPassword();
    await appendPassword('word');
    await submitForm();
    expect(await getLoginResult()).toBe('Success');
  });
});

async function setUsername(username: string) {
  const usernameField = await app.findElementByTestID('username-field');
  await usernameField.setValue(username);
}

async function setPassword(password: string) {
  const passwordField = await app.findElementByTestID('password-field');
  await passwordField.setValue(password);
}

async function appendPassword(password: string) {
  const passwordField = await app.findElementByTestID('password-field');
  await passwordField.addValue('End');
  await passwordField.addValue(password);
}

async function toggleShowPassword() {
  const showPasswordToggle = await app.findElementByTestID(
    'show-password-toggle',
  );
  await showPasswordToggle.click();
}

async function submitForm() {
  const submitButton = await app.findElementByTestID('submit-button');
  await submitButton.click();
}

async function getLoginResult(): Promise<string> {
  const loginResult = await app.findElementByTestID('result-text');
  return await loginResult.getText();
}
