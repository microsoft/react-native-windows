/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {NativeEventEmitter, NativeModules} from 'react-native';
import {IHighContrastColors, IHighContrastChangedEvent} from './AppThemeTypes';

const invariant = require('invariant');
const NativeAppTheme = NativeModules.RTCAppTheme;

class AppThemeModule extends NativeEventEmitter {
  public isAvailable: boolean;
  private _isHighContrast: boolean;
  private _highContrastColors: IHighContrastColors;

  constructor() {
    super(NativeAppTheme);
    this.isAvailable = true;

    this._highContrastColors = NativeAppTheme.initialHighContrastColors;
    this._isHighContrast = NativeAppTheme.initialHighContrast;
    this.addListener(
      'highContrastChanged',
      (nativeEvent: IHighContrastChangedEvent) => {
        this._isHighContrast = nativeEvent.isHighContrast;
        this._highContrastColors = nativeEvent.highContrastColors;
      },
    );
  }

  get isHighContrast(): boolean {
    return this._isHighContrast;
  }

  get currentHighContrastColors(): IHighContrastColors {
    return this._highContrastColors;
  }
}

function throwMissingNativeModule() {
  invariant(
    false,
    'Cannot use AppTheme module when native RTCAppTheme is not included in the build.\n' +
      'Either include it, or check AppTheme.isAvailable before calling any methods.',
  );
}

// This module depends on the native `RCTAppTheme` module. If you don't include it,
// `AppTheme.isAvailable` will return `false`, and any method calls will throw.
class MissingNativeAppThemeShim {
  public isAvailable = false;
  public isHighContrast = false;
  public currentHighContrastColors = {} as IHighContrastColors;

  addEventListener() {
    throwMissingNativeModule();
  }

  removeEventListener() {
    throwMissingNativeModule();
  }

  // EventEmitter
  addListener(
    _eventType: string,
    _listener: (nativeEvent: IHighContrastChangedEvent) => void,
  ): any {
    throwMissingNativeModule();
  }

  removeAllListeners() {
    throwMissingNativeModule();
  }

  removeListener(
    _eventType: string,
    _listener: (nativeEvent: IHighContrastChangedEvent) => void,
  ) {
    throwMissingNativeModule();
  }

  removeSubscription() {
    throwMissingNativeModule();
  }
}

export type AppTheme = AppThemeModule;
export const AppTheme = NativeAppTheme
  ? new AppThemeModule()
  : new MissingNativeAppThemeShim();
