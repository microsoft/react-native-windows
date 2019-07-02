/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * Portions copyright for react-native-windows:
 *
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @flow
 */
'use strict';

var RCTDeviceEventEmitter = require('RCTDeviceEventEmitter');

var TOUCH_EXPLORATION_EVENT = 'touchExplorationDidChange';

type ChangeEventName = $Enum<{
  change: string,
}>;

var _subscriptions = new Map();

var AccessibilityInfo = {
  fetch: function(): Promise {
    return new Promise((resolve, reject) => {
      // TODO Hx: Implement this module.
      return false;
    });
  },

  addEventListener: function(
    eventName: ChangeEventName,
    handler: Function,
  ): void {
    // TODO Hx: Implement this module.
    var listener = RCTDeviceEventEmitter.addListener(
      TOUCH_EXPLORATION_EVENT,
      enabled => {
        handler(enabled);
      },
    );
    _subscriptions.set(handler, listener);
  },

  removeEventListener: function(
    eventName: ChangeEventName,
    handler: Function,
  ): void {
    // TODO Hx: Implement this module.
    var listener = _subscriptions.get(handler);
    if (!listener) {
      return;
    }
    listener.remove();
    _subscriptions.delete(handler);
  },
};

module.exports = AccessibilityInfo;
