/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

<<<<<<< Upstream
import type {DialogOptions} from '../NativeModules/specs/NativeDialogManagerAndroid';

import Platform from '../Utilities/Platform';
import {alertWithArgs} from './RCTAlertManager';
=======
// [Windows
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';
// $FlowFixMe[underconstrained-implicit-instantiation]
const PLYAlertManager = TurboModuleRegistry.getEnforcing('Alert');
// Windows]
>>>>>>> Override

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
 * See https://reactnative.dev/docs/alert
 */
class Alert {
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
<<<<<<< Upstream
    if (Platform.OS === 'ios') {
      let callbacks: Array<?any> = [];
      const buttons = [];
      let cancelButtonKey;
      let destructiveButtonKey;
      let preferredButtonKey;
      if (typeof callbackOrButtons === 'function') {
        callbacks = [callbackOrButtons];
      } else if (Array.isArray(callbackOrButtons)) {
        callbackOrButtons.forEach((btn, index) => {
          callbacks[index] = btn.onPress;
          if (btn.style === 'cancel') {
            cancelButtonKey = String(index);
          } else if (btn.style === 'destructive') {
            destructiveButtonKey = String(index);
          }
          if (btn.isPreferred) {
            preferredButtonKey = String(index);
          }
          if (btn.text || index < (callbackOrButtons || []).length - 1) {
            const btnDef: {[number]: string} = {};
            btnDef[index] = btn.text || '';
            buttons.push(btnDef);
          }
        });
      }

      alertWithArgs(
        {
          title: title || '',
          message: message || undefined,
          buttons,
          type: type || undefined,
          defaultValue,
          cancelButtonKey,
          destructiveButtonKey,
          preferredButtonKey,
          keyboardType,
          userInterfaceStyle: options?.userInterfaceStyle || undefined,
        },
        (id, value) => {
          const cb = callbacks[id];
          cb && cb(value);
        },
      );
    }
=======
    // [Windows
    throw new Error(
      'Alert.prompt not currently implemented in react-native-win32',
    );
    // Windows]
>>>>>>> Override
  }
}

export default Alert;
