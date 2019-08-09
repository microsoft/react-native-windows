/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {NativeEventEmitter, NativeModules} from 'react-native';
const MissingNativeEventEmitterShim = require('MissingNativeEventEmitterShim');
import {
  AppThemeTypes,
  IHighContrastColors,
  IHighContrastChangedEvent,
} from './AppThemeTypes';

const NativeAppTheme = NativeModules.RTCAppTheme;

class AppThemeModule extends NativeEventEmitter {
  public isAvailable: boolean;
  private _isHighContrast: boolean;
  private _currentTheme: AppThemeTypes;
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

    this._currentTheme = NativeAppTheme.initialAppTheme;
    this.addListener(
      'appThemeChanged',
      ({currentTheme}: {currentTheme: AppThemeTypes}) => {
        this._currentTheme = currentTheme;
      },
    );
  }

  get currentTheme(): AppThemeTypes {
    return this._currentTheme;
  }

  get isHighContrast(): boolean {
    return this._isHighContrast;
  }

  get currentHighContrastColors(): IHighContrastColors {
    return this._highContrastColors;
  }
}

// This module depends on the native `RCTAppTheme` module. If you don't include it,
// `AppTheme.isAvailable` will return `false`, and any method calls will throw.
class MissingNativeAppThemeShim extends MissingNativeEventEmitterShim {
  public isAvailable = false;
  public currentTheme = '';
  public isHighContrast = false;
  public currentHighContrastColors = {} as IHighContrastColors;
}

export const AppTheme = NativeAppTheme
  ? new AppThemeModule()
  : new MissingNativeAppThemeShim();
export default AppTheme;
