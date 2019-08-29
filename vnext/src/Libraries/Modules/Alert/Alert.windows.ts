/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {NativeModules} from 'react-native';
const AlertNative = NativeModules.Alert;

export type Buttons = Array<{
  text?: string;
  onPress?: Function | null;
  style?: string;
}>;

type Options = {
  cancelable?: boolean | null;
  onDismiss?: Function | null;
};

export class Alert {
  public static alert(
    title: string | null,
    message?: string | null,
    buttons?: Buttons,
    options?: Options,
  ) {
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
}

module.exports = Alert;
