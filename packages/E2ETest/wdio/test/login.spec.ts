/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import LoginPage from '../pages/LoginPage';
import HomePage from '../pages/HomePage';
import { LOGIN_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(async () => {
  await HomePage.goToComponentExample(LOGIN_TESTPAGE);
});

describe('LoginTest', async () => {
  it('Login Success', async () => {
    await LoginPage.setLoginInfo('username', 'password');
    await LoginPage.submitForm();
    expect(await LoginPage.getLoginResult()).toBe('Success');
  });

  it('Login Fail due to user email', async () => {
    await LoginPage.setLoginInfo('username@microsoft.com', 'password');
    await LoginPage.submitForm();
    expect(await LoginPage.getLoginResult()).toBe('Fail');
  });

  it('Login Fail due to wrong password', async () => {
    await LoginPage.setLoginInfo('username', 'abcdefg');
    await LoginPage.submitForm();
    expect(await LoginPage.getLoginResult()).toBe('Fail');
  });

  it('Login Success with secureTextEntry off', async () => {
    await LoginPage.toggleShowPassword();
    await LoginPage.setLoginInfo('username', 'password');
    await LoginPage.submitForm();
    expect(await LoginPage.getLoginResult()).toBe('Success');
  });

  it('Login Success with secureTextEntry off then on', async () => {
    await LoginPage.setLoginInfo('username', 'pass');
    await LoginPage.toggleShowPassword();
    await LoginPage.appendPassword('word');
    await LoginPage.submitForm();
    expect(await LoginPage.getLoginResult()).toBe('Success');
  });

  it('Login Success with secureTextEntry on then off', async () => {
    await LoginPage.setLoginInfo('username', 'pass');
    await LoginPage.toggleShowPassword();
    await LoginPage.appendPassword('word');
    await LoginPage.submitForm();
    expect(await LoginPage.getLoginResult()).toBe('Success');
  });
});
