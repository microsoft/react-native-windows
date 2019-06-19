// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter as NativeEventEmitter_TypeCarrier, EmitterSubscription, NativeEventEmitter } from 'react-native';
const RCTDeviceEventEmitter: typeof NativeEventEmitter_TypeCarrier = require('RCTDeviceEventEmitter');
const ThemingNative = require('NativeModules').ThemingModule;

const _subscriptions = new Map<Function, EmitterSubscription>();

class Theming extends NativeEventEmitter  {

  constructor() {
    super(ThemingNative);
  }

  addEventListener(type: string, handler: Function) {
    // TODO Hx: Implement this module.
    const listener = RCTDeviceEventEmitter.addListener('themeOrContrastDidChange', (enabledInner: boolean) => {
      handler(enabledInner);
    });
    _subscriptions.set(handler, listener);
  }

  removeEventListener(eventName: 'themeOrContrastDidChange', handler: Function) {
    // TODO Hx: Implement this module.
    const listener = _subscriptions.get(handler);
    if (!listener) {
      return;
    }
    listener.remove();
    _subscriptions.delete(handler);
  }

  getConstants() {
    return ThemingNative.isHighContrast;
  }

  getMethods() {
    return ThemingNative.getCurrentTheme();
  }
};

export = Theming;