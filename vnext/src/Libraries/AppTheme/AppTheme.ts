/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {
  AppThemeTypes,
  IAppThemeChangedEvent,
  IHighContrastColors,
  IHighContrastChangedEvent,
} from './AppThemeTypes';
const invariant = require('invariant');

function throwMissingNativeModule() {
  invariant(
    false,
    'Cannot use AppTheme module when native RTCAppTheme is not included in the build.\n' +
      'Either include it, or check AppTheme.isAvailable before calling any methods.',
  );
}

// This module depends on the native `RCTAppTheme` module. If you don't include it,
// `AppTheme.isAvailable` will return `false`, and any method calls will throw.
class AppThemeModule {
  public isAvailable = false;
  public currentTheme: AppThemeTypes = 'light';
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
    _listener: (
      nativeEvent: IAppThemeChangedEvent & IHighContrastChangedEvent,
    ) => void,
  ): any {
    throwMissingNativeModule();
  }

  removeAllListeners() {
    throwMissingNativeModule();
  }

  removeListener(
    _eventType: string,
    _listener: (
      nativeEvent: IAppThemeChangedEvent & IHighContrastChangedEvent,
    ) => void,
  ) {
    throwMissingNativeModule();
  }

  removeSubscription() {
    throwMissingNativeModule();
  }
}

export const AppTheme = new AppThemeModule();
export type AppTheme = AppThemeModule;
