/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 */
'use strict';

var RCTDeviceEventEmitter = require('../../EventEmitter/RCTDeviceEventEmitter');

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
