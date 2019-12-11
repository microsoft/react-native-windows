/**
 * @providesModule Alert
 */
'use strict';
/* tslint:disable:no-any */

declare module 'react-native' {
  /* tslint:disable-next-line interface-name */
  interface NativeModulesStatic {
    /* tslint:disable-next-line no-any */
    PLYAlertManager: any;
  }
}

import * as NativeModules from 'react-native/Libraries/BatchedBridge/NativeModules';
import { AlertButton, AlertOptions } from 'react-native';

class Alert {
  // tslint:disable-next-line: function-name -- matching facebook name
  public static alert(title: string, content?: string, buttons?: AlertButton[], options?: AlertOptions) {
    NativeModules.PLYAlertManager.showAlert(title, content, buttons, options, buttonIndex => {
      if (buttonIndex >= 0 && buttonIndex < buttons.length) {
        buttons[buttonIndex].onPress();
      }

      if (options && options.onDismiss && buttonIndex === -1) {
        options.onDismiss();
      }
    });
  }
}

export = Alert;
