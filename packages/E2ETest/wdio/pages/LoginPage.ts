/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import { BasePage, by } from './BasePage';
import {
  USERNAME_ON_LOGIN,
  PASSWORD_ON_LOGIN,
  SUBMIT_ON_LOGIN,
  LOGINRESULT_ON_LOGIN,
  SHOWPASSWORD_ON_LOGIN,
} from '@react-native-windows/tester/js/examples-win/LegacyTests/Consts';

class LoginPage extends BasePage {
  async setLoginInfo(userName: string, password: string) {
    await (await this._userName).setValue(userName);
    await (await this._password).setValue(password);
  }

  async appendPassword(password: string) {
    await (await this._password).addValue('End');
    await (await this._password).addValue(password);
  }

  async toggleShowPassword() {
    await (await this._showPassword).click();
  }

  async submitForm() {
    await (await this._submit).click();
  }

  async getLoginResult() {
    return (await this._loginResult).getText();
  }

  private get _userName() {
    return by(USERNAME_ON_LOGIN);
  }

  private get _password() {
    return by(PASSWORD_ON_LOGIN);
  }

  private get _submit() {
    return by(SUBMIT_ON_LOGIN);
  }

  private get _showPassword() {
    return by(SHOWPASSWORD_ON_LOGIN);
  }

  private get _loginResult() {
    return by(LOGINRESULT_ON_LOGIN);
  }
}

export default new LoginPage();
