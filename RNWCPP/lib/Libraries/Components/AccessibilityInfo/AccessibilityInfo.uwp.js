// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';
var RCTDeviceEventEmitter = require('RCTDeviceEventEmitter');
var TOUCH_EXPLORATION_EVENT = 'touchExplorationDidChange';
var _subscriptions = new Map();
var AccessibilityInfo = {
    fetch: function () {
        // tslint:disable-next-line no-any
        return new Promise(function (resolve, reject) {
            // TODO Hx: Implement this module.
            return false;
        });
    },
    addEventListener: function (eventName, handler) {
        // TODO Hx: Implement this module.
        var listener = RCTDeviceEventEmitter.addListener(TOUCH_EXPLORATION_EVENT, function (enabledInner) {
            handler(enabledInner);
        });
        _subscriptions.set(handler, listener);
    },
    removeEventListener: function (eventName, handler) {
        // TODO Hx: Implement this module.
        var listener = _subscriptions.get(handler);
        if (!listener) {
            return;
        }
        listener.remove();
        _subscriptions.delete(handler);
    }
};
module.exports = AccessibilityInfo;
//# sourceMappingURL=AccessibilityInfo.uwp.js.map