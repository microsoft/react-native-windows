/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @flow
 * @format
 */

import type {DialogOptions} from './NativeDialogManagerWindows';
const NativeDialogManagerWindows =
  require('./NativeDialogManagerWindows').default;

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
    if (!NativeDialogManagerWindows) {
      return;
    }
    const constants = NativeDialogManagerWindows.getConstants();

    const config: DialogOptions = {
      title: title || '',
      message: message || '',
      cancelable: false,
    };

    if (options && options.cancelable) {
      config.cancelable = options.cancelable;
    }
    // At most three buttons (neutral, negative, positive). Ignore rest.
    // The text 'OK' should be probably localized. iOS Alert does that in native.
    const defaultPositiveText = 'OK';
    const validButtons: Buttons = buttons
      ? buttons.slice(0, 3)
      : [{text: defaultPositiveText, style: 'default'}];
    const buttonPositive = validButtons.pop();
    const buttonNegative = validButtons.pop();
    const buttonNeutral = validButtons.pop();

    // Find the first button where 'default' is set to true
    // in order of declared buttons.
    const defaultIndex = [
      buttonNeutral,
      buttonNegative,
      buttonPositive,
    ].findIndex(b => b != null && b.style === 'default');

    // XAML has an enum to specify the default button, which is:
    //   None = 0, Primary = 1, Secondary = 2, Close = 3
    // If no default button is found, specify 0 for None, otherwise
    // convert the index to its corresponding XAML enum value.
    config.defaultButton = defaultIndex >= 0 ? 3 - defaultIndex : 0;

    if (buttonNeutral) {
      config.buttonNeutral = buttonNeutral.text || '';
    }
    if (buttonNegative) {
      config.buttonNegative = buttonNegative.text || '';
    }
    if (buttonPositive) {
      config.buttonPositive = buttonPositive.text || defaultPositiveText;
    }

    /* $FlowFixMe[missing-local-annot] The type annotation(s) required by
     * Flow's LTI update could not be added via codemod */
    const onAction = (action, buttonKey) => {
      if (action === constants.buttonClicked) {
        if (buttonKey === constants.buttonNeutral) {
          buttonNeutral.onPress && buttonNeutral.onPress();
        } else if (buttonKey === constants.buttonNegative) {
          buttonNegative.onPress && buttonNegative.onPress();
        } else if (buttonKey === constants.buttonPositive) {
          buttonPositive.onPress && buttonPositive.onPress();
        }
      } else if (action === constants.dismissed) {
        options && options.onDismiss && options.onDismiss();
      }
    };
    const onError = errorMessage => console.warn(errorMessage);
    NativeDialogManagerWindows.showAlert(config, onError, onAction);
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
    throw new Error(
      'Alert.prompt not currently implemented in react-native-windows',
    );
  }
}

module.exports = Alert;
