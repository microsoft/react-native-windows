/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @flow
 * @format
 */
'use strict';

const NativeModules = require('../BatchedBridge/NativeModules');
const AlertNative = NativeModules.Alert;

export type Buttons = Array<{
  text?: string,
  onPress?: ?Function,
  style?: AlertButtonStyle,
  ...
}>;

type Options = {
  cancelable?: ?boolean,
  onDismiss?: ?() => void,
  ...
};

export type AlertType = $Keys<{
  default: string,
  'plain-text': string,
  'secure-text': string,
  'login-password': string,
}>;

export type AlertButtonStyle = $Keys<{
  default: string,
  cancel: string,
  destructive: string,
}>;

class Alert {
  static alert(
    title: ?string,
    message?: ?string,
    buttons?: Buttons,
    options?: Options,
  ): void {
    // At most three buttons (neutral, negative, positive). Ignore rest.
    // The text 'OK' should be probably localized. iOS Alert does that in native.
    const validButtons: Buttons = buttons
      ? buttons.slice(0, 3)
      : [{text: 'OK'}];
    const buttonPositive = validButtons.pop();
    const buttonNegative = validButtons.pop();
    const buttonNeutral = validButtons.pop();

    let args = {
      title: title || '',
      message: message || '',
      cancelable: options ? options.cancelable : null,
      buttonNeutral: buttonNeutral ? buttonNeutral.text : '',
      buttonNegative: buttonNegative ? buttonNegative.text : '',
      buttonPositive: buttonPositive ? buttonPositive.text : '',
    };

    AlertNative.showAlert(args, (actionResult: string) => {
      if (actionResult === 'neutral') {
        buttonNeutral && buttonNeutral.onPress && buttonNeutral.onPress();
      } else if (actionResult === 'negative') {
        buttonNegative && buttonNegative.onPress && buttonNegative.onPress();
      } else if (actionResult === 'positive') {
        buttonPositive && buttonPositive.onPress && buttonPositive.onPress();
      }
    });
  }

  static prompt(
    title: ?string,
    message?: ?string,
    callbackOrButtons?: ?(((text: string) => void) | Buttons),
    type?: ?AlertType = 'plain-text',
    defaultValue?: string,
    keyboardType?: string,
  ): void {
    throw new Error(
      'Alert.prompt not currently implemented in react-native-windows',
    );
  }
}

module.exports = Alert;
