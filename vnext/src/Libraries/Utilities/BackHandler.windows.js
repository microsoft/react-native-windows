/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

// On Apple TV, this implements back navigation using the TV remote's menu button.
// On iOS, this just implements a stub.

'use strict';

const RCTDeviceEventEmitter = require('../EventEmitter/RCTDeviceEventEmitter');

// TODO Hx: Implement.

/**
 * Detect hardware button presses for back navigation.
 *
 * Android: Detect hardware back button presses, and programmatically invoke the default back button
 * functionality to exit the app if there are no listeners or if none of the listeners return true.
 *
 * tvOS: Detect presses of the menu button on the TV remote.  (Still to be implemented:
 * programmatically disable menu button handling
 * functionality to exit the app if there are no listeners or if none of the listeners return true.)
 *
 * iOS: Not applicable.
 *
 * The event subscriptions are called in reverse order (i.e. last registered subscription first),
 * and if one subscription returns true then subscriptions registered earlier will not be called.
 *
 * Example:
 *
 * ```javascript
 * BackHandler.addEventListener('hardwareBackPress', function() {
 *  // this.onMainScreen and this.goBack are just examples, you need to use your own implementation here
 *  // Typically you would use the navigator here to go to the last state.
 *
 *  if (!this.onMainScreen()) {
 *    this.goBack();
 *    return true;
 *  }
 *  return false;
 * });
 * ```
 */

type BackPressEventName = 'backPress' | 'hardwareBackPress';

const DEVICE_BACK_EVENT = 'hardwareBackPress';

const _backListeners = [];

RCTDeviceEventEmitter.addListener(DEVICE_BACK_EVENT, () => {
  for (let i = _backListeners.length - 1; i >= 0; i--) {
    if (_backListeners[i]()) {
      return;
    }
  }

  BackHandler.exitApp();
});

type TBackHandler = {|
  +exitApp: () => void,
  +addEventListener: (
    eventName: BackPressEventName,
    handler: Function,
  ) => {remove: () => void, ...},
  +removeEventListener: (
    eventName: BackPressEventName,
    handler: Function,
  ) => void,
|};

const BackHandler: TBackHandler = {
  exitApp: () => {},
  addEventListener: (eventName: BackPressEventName, handler: Function) => {
    debugger;
    if (_backListeners.indexOf(handler) === -1) {
      _backListeners.push(handler);
    }

    return {
      remove: () => {
        BackHandler.removeEventListener(eventName, handler);
      },
    };
  },
  removeEventListener: (eventName: BackPressEventName, handler: Function) => {
    const handlerIndex = _backListeners.indexOf(handler);

    if (handlerIndex !== -1) {
      _backListeners.splice(handlerIndex, 1);
    }
  },
};

module.exports = BackHandler;
