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
 * @providesModule AccessibilityInfo
 * @flow
 */
'use strict';

var NativeModules = require('NativeModules');
var RCTDeviceEventEmitter = require('RCTDeviceEventEmitter');

var RCTAccessibilityInfo = NativeModules.AccessibilityInfo;
var HIGH_CONTRAST_EVENT = 'highContrastDidChange';
var _subscriptions = new Map();

type AccessibilityEventName = $Enum<{
  [HIGH_CONTRAST_EVENT]: string,
}>;

var AccessibilityInfo = {

  initialHighContrast: RCTAccessibilityInfo.initialHighContrast,
  
  fetch: function(): Promise {
    return new Promise((resolve, reject) => {
      resolve(false); // UWP does not have API to check if screen reader is being used
    });
  },

  fetchIsHighContrast: function (): Promise {
    return RCTAccessibilityInfo.fetchIsHighContrast();
  },  

  addEventListener: function (
    eventName: AccessibilityEventName,
    handler: Function
  ): Object {
    if (eventName !== HIGH_CONTRAST_EVENT) {
      return {
        remove() { }
      };      
    }

    var listener = RCTDeviceEventEmitter.addListener(eventName, enabled => handler(enabled));

    _subscriptions.set(handler, listener);

    return listener;
  },

  removeEventListener: function(
    eventName: AccessibilityEventName,
    handler: Function
  ): void {
    var listener = _subscriptions.get(handler);
    listener && listener.remove();
    _subscriptions.delete(handler);  
  },

};

module.exports = AccessibilityInfo;
