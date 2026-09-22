/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

// [Windows
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';
// $FlowFixMe[underconstrained-implicit-instantiation]
const PLYAlertManager = TurboModuleRegistry.getEnforcing('Alert');
// Windows]

/**
 * @platform ios
 */
export type AlertType =
  | 'default'
  | 'plain-text'
  | 'secure-text'
  | 'login-password';

/**
 * @platform ios
 */
export type AlertButtonStyle = 'default' | 'cancel' | 'destructive';

export type AlertButton = {
  text?: string,
  onPress?: ?((value?: string) => any) | ?Function,
  isPreferred?: boolean,
  style?: AlertButtonStyle,
  ...
};

export type AlertButtons = Array<AlertButton>;

export type AlertOptions = {
  /** @platform android */
  cancelable?: ?boolean,
  userInterfaceStyle?: 'unspecified' | 'light' | 'dark',
  /** @platform android */
  onDismiss?: ?() => void,
  ...
};

/**
 * Launches an alert dialog with the specified title and message.
 *
 * Optionally provide a list of buttons. Tapping any button will fire the
 * respective onPress callback and dismiss the alert. By default, the only
 * button will be an 'OK' button.
 *
 * This is an API that works both on iOS and Android and can show static
 * alerts. On iOS, you can show an alert that prompts the user to enter
 * some information.
 *
 * ## iOS
 *
 * On iOS you can specify any number of buttons. Each button can optionally
 * specify a style, which is one of 'default', 'cancel' or 'destructive'.
 *
 * ## Android
 *
 * On Android at most three buttons can be specified. Android has a concept
 * of a neutral, negative and a positive button:
 *
 *   - If you specify one button, it will be the 'positive' one (such as 'OK')
 *   - Two buttons mean 'negative', 'positive' (such as 'Cancel', 'OK')
 *   - Three buttons mean 'neutral', 'negative', 'positive' (such as 'Later', 'Cancel', 'OK')
 *
 * Example:
 *
 * ```tsx
 * // Works on both iOS and Android
 * Alert.alert(
 *   'Alert Title',
 *   'My Alert Msg',
 *   [
 *     {text: 'Ask me later', onPress: () => console.log('Ask me later pressed')},
 *     {text: 'Cancel', onPress: () => console.log('Cancel Pressed'), style: 'cancel'},
 *     {text: 'OK', onPress: () => console.log('OK Pressed')},
 *   ]
 * )
 * ```
 *
 * @see https://reactnative.dev/docs/alert
 */
class Alert {
  /**
   * Display an alert dialog with the specified title, message, and buttons.
   * On Android, at most three buttons can be specified. On iOS, any number of
   * buttons can be used.
   */
  static alert(
    title: ?string,
    message?: ?string,
    buttons?: AlertButtons,
    options?: AlertOptions,
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

  /**
   * Create and display a prompt to enter text. Accepts a title, message,
   * callback or buttons, input type, default value, keyboard type, and options.
   *
   * @platform ios
   */
  static prompt(
    title: ?string,
    message?: ?string,
    callbackOrButtons?: ?(((text: string) => void) | AlertButtons),
    type?: ?AlertType = 'plain-text',
    defaultValue?: string,
    keyboardType?: string,
    options?: AlertOptions,
  ): void {
    // [Windows
    throw new Error(
      'Alert.prompt not currently implemented in react-native-win32',
    );
    // Windows]
  }
}

export default Alert;
