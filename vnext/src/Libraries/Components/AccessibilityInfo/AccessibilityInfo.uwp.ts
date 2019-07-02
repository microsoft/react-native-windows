// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter as NativeEventEmitter_TypeCarrier, EmitterSubscription } from 'react-native';
const RCTDeviceEventEmitter: typeof NativeEventEmitter_TypeCarrier = require('RCTDeviceEventEmitter');

const TOUCH_EXPLORATION_EVENT = 'touchExplorationDidChange';

type ChangeEventName = 'change';

const _subscriptions = new Map<Function, EmitterSubscription>();

const AccessibilityInfo = {
  fetch: (): Promise<boolean> => {
    // tslint:disable-next-line no-any
    return new Promise((_resolve:(value?: boolean | PromiseLike<boolean>) => void, _reject: (reason?: any) => void) => {
      // TODO Hx: Implement this module.
      return false;
    });
  },

  addEventListener: (eventName: ChangeEventName, handler:((enabled: boolean) => void)) => {
    // TODO Hx: Implement this module.
    const listener = RCTDeviceEventEmitter.addListener(TOUCH_EXPLORATION_EVENT, (enabledInner: boolean) => {
      handler(enabledInner);
    });
    _subscriptions.set(handler, listener);
  },

  removeEventListener: (eventName: ChangeEventName, handler: Function): void => {
    // TODO Hx: Implement this module.
    const listener = _subscriptions.get(handler);
    if (!listener) {
      return;
    }
    listener.remove();
    _subscriptions.delete(handler);
  },
};

export = AccessibilityInfo;
