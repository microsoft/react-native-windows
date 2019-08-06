/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {NativeModules} from 'react-native';
const AlertNative = NativeModules.RCTAlert;

export class Alert {
  public static showAlert() {
    AlertNative.showAlert('test');
  }
}

export default Alert;
