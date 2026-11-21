/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<ce819e59e9eb4e9823f485dc87588280>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Alert\Alert.js
 */

/**
 * @platform ios
 */
export type AlertType = "default" | "plain-text" | "secure-text" | "login-password";
/**
 * @platform ios
 */
export type AlertButtonStyle = "default" | "cancel" | "destructive";
export type AlertButton = {
  text?: string | undefined;
  onPress?: (((value?: string) => any) | undefined) | (Function | undefined) | undefined;
  isPreferred?: boolean | undefined;
  style?: AlertButtonStyle | undefined;
};
export type AlertButtons = Array<AlertButton>;
export type AlertOptions = {
  /** @platform android */
  cancelable?: boolean | undefined;
  userInterfaceStyle?: "unspecified" | "light" | "dark" | undefined;
  /** @platform android */
  onDismiss?: (() => void) | undefined;
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
declare class Alert {
  /**
   * Display an alert dialog with the specified title, message, and buttons.
   * On Android, at most three buttons can be specified. On iOS, any number of
   * buttons can be used.
   */
  static alert(title: null | undefined | string, message?: null | undefined | string, buttons?: AlertButtons, options?: AlertOptions): void;
  /**
   * Create and display a prompt to enter text. Accepts a title, message,
   * callback or buttons, input type, default value, keyboard type, and options.
   *
   * @platform ios
   */
  static prompt(title: null | undefined | string, message?: null | undefined | string, callbackOrButtons?: null | undefined | (((text: string) => void) | AlertButtons), type?: null | undefined | AlertType, defaultValue?: string, keyboardType?: string, options?: AlertOptions): void;
}
export default Alert;
