/**
 * Copyright (c) 2015-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * Portions copyright for react-native-windows:
 * 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * 
 * @providesModule Alert
 * @flow
 */
'use strict';

const NativeModules = require('NativeModules');

export type Buttons = Array<{
  text?: string,
  onPress?: ?Function,
  style?: AlertButtonStyle,
}>;

type Options = {
  cancelable?: ?boolean,
  onDismiss?: ?Function,
  rootViewHint?: ?number,
};

/**
 * Launches an alert dialog with the specified title and message.
 *
 * See http://facebook.github.io/react-native/docs/alert.html
 *
 * ## Windows
 *
 * On Windows at most two buttons can be specified.
 *
 *   - If you specify one button, it will be the 'positive' one (such as 'OK')
 *   - Two buttons mean 'negative', 'positive' (such as 'Cancel', 'OK')
 *
 * An optional parameter named `rootViewHint` (a tag of an element that can identify the window that should parent the alert)
 * is also available
 *
 */
class Alert {

  /**
   * Launches an alert dialog with the specified title and message.
   * 
   * See http://facebook.github.io/react-native/docs/alert.html#alert
   */
  static alert(
    title: ?string,
    message?: ?string,
    buttons?: Buttons,
    options?: Options,
    type?: AlertType,
  ): void {
    AlertWindows.alert(title, message, buttons, options);
  }
}

/**
 * Wrapper around the Windows native module.
 */
class AlertWindows {

  static alert(
    title: ?string,
    message?: ?string,
    buttons?: Buttons,
    options?: Options,
  ): void {
    var config = {
      title: title || '',
      message: message || '',
    };
    // At most two buttons (negative, positive). Ignore rest. The text
    // 'OK' should be probably localized. iOS Alert does that in native.
    var validButtons: Buttons = buttons ? buttons.slice(0, 2) : [{text: 'OK'}];
    var buttonPositive = validButtons.pop();
    var buttonNegative = validButtons.pop();
    if (buttonNegative) {
      config = {...config, buttonNegative: buttonNegative.text || '' };
    }
    if (buttonPositive) {
      config = {...config, buttonPositive: buttonPositive.text || '' };
    }
    if (options) {
      config = {...config, rootViewHint: options.rootViewHint};
    }
    NativeModules.DialogManagerWindows.showAlert(
      config,
      (errorMessage) => console.warn(errorMessage),
      (action, buttonKey) => {
        if (action !== NativeModules.DialogManagerWindows.buttonClicked) {
          return;
        }
        if (buttonKey === NativeModules.DialogManagerWindows.buttonNegative) {
          buttonNegative.onPress && buttonNegative.onPress();
        } else if (buttonKey === NativeModules.DialogManagerWindows.buttonPositive) {
          buttonPositive.onPress && buttonPositive.onPress();
        }
      }
    );
  }
}

module.exports = Alert;
