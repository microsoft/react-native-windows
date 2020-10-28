/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import LoginPage from '../pages/LoginPage';
import HomePage from '../pages/HomePage';
import { LOGIN_TESTPAGE } from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

beforeAll(() => {
  HomePage.goToComponentExample(LOGIN_TESTPAGE);
});

describe('LoginTest', () => {
  it('Login Success', () => {
    LoginPage.setLoginInfo('username', 'password');
    LoginPage.submitForm();
    expect(LoginPage.getLoginResult()).toBe('Success');
  });

  it('Login Fail due to user email', () => {
    LoginPage.setLoginInfo('username@microsoft.com', 'password');
    LoginPage.submitForm();
    expect(LoginPage.getLoginResult()).toBe('Fail');
  });

  it('Login Fail due to wrong password', () => {
    LoginPage.setLoginInfo('username', 'abcdefg');
    LoginPage.submitForm();
    expect(LoginPage.getLoginResult()).toBe('Fail');
  });

  it('Login Success with secureTextEntry off', () => {
    LoginPage.toggleShowPassword();
    LoginPage.setLoginInfo('username', 'password');
    LoginPage.submitForm();
    expect(LoginPage.getLoginResult()).toBe('Success');
  });

  it('Login Success with secureTextEntry off then on', () => {
    LoginPage.setLoginInfo('username', 'pass');
    LoginPage.toggleShowPassword();
    LoginPage.appendPassword('word');
    LoginPage.submitForm();
    expect(LoginPage.getLoginResult()).toBe('Success');
  });

  it('Login Success with secureTextEntry on then off', () => {
    LoginPage.setLoginInfo('username', 'pass');
    LoginPage.toggleShowPassword();
    LoginPage.appendPassword('word');
    LoginPage.submitForm();
    expect(LoginPage.getLoginResult()).toBe('Success');
  });
});
