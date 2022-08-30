/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

// [Windows
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';
const PLYAlertManager = TurboModuleRegistry.getEnforcing('Alert');
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
  ...
}>;

type Options = {
  cancelable?: ?boolean,
  userInterfaceStyle?: 'unspecified' | 'light' | 'dark',
  onDismiss?: ?() => void,
  ...
};

/**
 * Launches an alert dialog with the specified title and message.
 *
 * See https://reactnative.dev/docs/alert
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
    options?: Options,
  ): void {
    // [Windows
    throw new Error(
      'Alert.prompt not currently implemented in react-native-win32',
    );
    // Windows]
  }
}

module.exports = Alert;
