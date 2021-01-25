/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { goToComponentExample } from './Helpers';

beforeAll(async () => {
  await goToComponentExample('LegacyLoginTest');
});

describe('LoginTest', () => {
  it('Login Success', async () => {
    await setUsername('username');
    await setPassword('password');

    await submitForm();
    expect(await getLoginResult()).toBe('Success');
  });

  it('Login Fail due to user email', async () => {
    await setUsername('username@microsoft.com');
    await setPassword('password');

    await submitForm();
    expect(await getLoginResult()).toBe('Fail');
  });

  it('Login Fail due to wrong password', async () => {
    await setUsername('username');
    await setPassword('abcdefg');

    await submitForm();
    expect(await getLoginResult()).toBe('Fail');
  });

  it('Login Success with secureTextEntry off', async () => {
    await toggleShowPassword();
    await setUsername('username');
    await setPassword('password');

    await submitForm();
    expect(await getLoginResult()).toBe('Success');
  });

  it('Login Success with secureTextEntry off then on', async () => {
    await setUsername('username');
    await setPassword('pass');

    await toggleShowPassword();
    await appendPassword('word');
    await submitForm();
    expect(await getLoginResult()).toBe('Success');
  });

  it('Login Success with secureTextEntry on then off', async () => {
    await setUsername('username');
    await setPassword('pass');

    await toggleShowPassword();
    await appendPassword('word');
    await submitForm();
    expect(await getLoginResult()).toBe('Success');
  });
});

async function setUsername(username: string) {
  const usernameField = await $('~username-field');
  await usernameField.setValue(username);
}

async function setPassword(password: string) {
  const passwordField = await $('~password-field');
  await passwordField.setValue(password);
}

async function appendPassword(password: string) {
  const passwordField = await $('~password-field');
  await passwordField.addValue('End');
  await passwordField.addValue(password);
}

async function toggleShowPassword() {
  const showPasswordToggle = await $('~show-password-toggle');
  await showPasswordToggle.click();
}

async function submitForm() {
  const submitButton = await $('~submit-button');
  await submitButton.click();
}

async function getLoginResult(): Promise<string> {
  const loginResult = await $('~result-text');
  return await loginResult.getText();
}
