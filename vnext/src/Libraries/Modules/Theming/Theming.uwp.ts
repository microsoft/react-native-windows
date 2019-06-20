// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter as NativeEventEmitter_TypeCarrier, EmitterSubscription, NativeEventEmitter } from 'react-native';
const RCTDeviceEventEmitter: typeof NativeEventEmitter_TypeCarrier = require('RCTDeviceEventEmitter');
const ThemingNative = require('NativeModules').ThemingModule;

const _subscriptions = new Map<Function, EmitterSubscription>();

export class Theming extends NativeEventEmitter  {

  constructor() {
    super(ThemingNative);
  }

  public addEventListener(eventName: 'themeOrContrastDidChange', handler: Function): void {
    // TODO Hx: Implement this module.
    const listener = RCTDeviceEventEmitter.addListener(eventName, (enabledInner: boolean) => {
      handler(enabledInner);
    });
    _subscriptions.set(handler, listener);
  }

  public removeEventListener(eventName: 'themeOrContrastDidChange', handler: Function): void {
    // TODO Hx: Implement this module.
    const listener = _subscriptions.get(handler);
    if (!listener) {
      return;
    }
    listener.remove();
    _subscriptions.delete(handler);
  }

  public getConstants(): string {
    return ThemingNative.isHighContrast;
  }

  public getMethods(): string {
    return ThemingNative.getCurrentTheme();
  }
}

export default Theming;