/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

// [Windows
const NativeModules = require('../BatchedBridge/NativeModules');
const PLYAlertManager = NativeModules.PLYAlertManager;
// Windows]

export type AlertType =
  | 'default'
  | 'plain-text'
  | 'secure-text'
  | 'login-password';
export type AlertButtonStyle = 'default' | 'cancel' | 'destructive';
export type Buttons = Array<{
  text?: string,
  onPress?: ?Function,
  style?: AlertButtonStyle,
}>;

type Options = {
  cancelable?: ?boolean,
  onDismiss?: ?() => void,
};

/**
 * Launches an alert dialog with the specified title and message.
 *
 * See http://facebook.github.io/react-native/docs/alert.html
 */
class Alert {
  static alert(
    title: ?string,
    message?: ?string,
    buttons?: Buttons,
    options?: Options,
  ): void {
    // [Windows
    PLYAlertManager.showAlert(
      title || '',
      message || '',
      buttons,
      options,
      buttonIndex => {
        if (
          buttonIndex >= 0 &&
          buttons &&
          buttonIndex < buttons.length &&
          buttons[buttonIndex].onPress
        ) {
          buttons[buttonIndex].onPress();
        }

        if (
          options &&
          options.onDismiss &&
          buttonIndex === -1 &&
          options.onDismiss
        ) {
          options.onDismiss();
        }
      },
    );
    // Windows]
  }

  static prompt(
    title: ?string,
    message?: ?string,
    callbackOrButtons?: ?(((text: string) => void) | Buttons),
    type?: ?AlertType = 'plain-text',
    defaultValue?: string,
    keyboardType?: string,
  ): void {
    // [Windows
    throw new Error(
      'Alert.prompt not currently implemented in react-native-win32',
    );
    // Windows]
  }
}

module.exports = Alert;
