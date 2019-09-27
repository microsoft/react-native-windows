/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */

import { BasePage, By } from './BasePage';
import {
  USERNAME_ON_LOGIN,
  PASSWORD_ON_LOGIN,
  SUBMIT_ON_LOGIN,
  LOGINRESULT_ON_LOGIN,
  SHOWPASSWORD_ON_LOGIN,
} from '../../app/Consts';

class LoginPage extends BasePage {
  isPageLoaded() {
    return super.isPageLoaded() && this._userName.isDisplayed();
  }

  setLoginInfo(userName: string, password: string) {
    this._userName.setValue(userName);
    this._password.setValue(password);
  }

  appendPassword(password: string) {
    this._password.addValue('End');
    this._password.addValue(password);
  }

  toggleShowPassword() {
    this._showPassword.click();
  }

  submitForm() {
    this._submit.click();
  }

  getLoginResult() {
    return this._loginResult.getText();
  }

  private get _userName() {
    return By(USERNAME_ON_LOGIN);
  }

  private get _password() {
    return By(PASSWORD_ON_LOGIN);
  }

  private get _submit() {
    return By(SUBMIT_ON_LOGIN);
  }

  private get _showPassword() {
    return By(SHOWPASSWORD_ON_LOGIN);
  }

  private get _loginResult() {
    return By(LOGINRESULT_ON_LOGIN);
  }
}

export default new LoginPage();
