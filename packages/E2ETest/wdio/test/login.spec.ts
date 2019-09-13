/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import LoginPage from '../pages/LoginPage';
import HomePage from '../pages/HomePage';
import assert from 'assert';

before(() => {
  HomePage.backToHomePage();
  HomePage.clickAndGotoLoginPage();
});

describe('LoginTest', () => {
  it('Login Success', () => {
    LoginPage.setLoginInfo('username', 'password');
    LoginPage.submitForm();
    assert.equal(LoginPage.getLoginResult(), 'Success');
  });

  it('Login Fail', () => {
    LoginPage.setLoginInfo('username@microsoft.com', 'password');
    LoginPage.submitForm();
    assert.equal(LoginPage.getLoginResult(), 'Fail');
  });

  it('Login Fail', () => {
    LoginPage.setLoginInfo('username', 'abcdefg');
    LoginPage.submitForm();
    assert.equal(LoginPage.getLoginResult(), 'Fail');
  });

  it('Login Success with secureTextEntry off', () => {
    LoginPage.toggleShowPassword();
    LoginPage.setLoginInfo('username', 'password');
    LoginPage.submitForm();
    assert.equal(LoginPage.getLoginResult(), 'Success');
  });

  it('Login Success with secureTextEntry off then on', () => {
    LoginPage.setLoginInfo('username', 'pass');
    LoginPage.toggleShowPassword();
    LoginPage.appendPassword('word');
    LoginPage.submitForm();
    assert.equal(LoginPage.getLoginResult(), 'Success');
  });

  it('Login Success with secureTextEntry on then off', () => {
    LoginPage.setLoginInfo('username', 'pass');
    LoginPage.toggleShowPassword();
    LoginPage.appendPassword('word');
    LoginPage.submitForm();
    assert.equal(LoginPage.getLoginResult(), 'Success');
  });
});
