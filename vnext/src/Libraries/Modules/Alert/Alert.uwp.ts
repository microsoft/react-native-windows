/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {NativeEventEmitter, NativeModules} from 'react-native';
const AlertNative = NativeModules.RCTAlert;

export class AlertModule extends NativeEventEmitter {
  constructor() {
    super(AlertNative);
  }
}

export const Alert = new AlertModule();
export default Alert;
