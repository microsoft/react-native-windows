/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {NativeEventEmitter, NativeModules} from 'react-native';
import {IHighContrastColors, IHighContrastChangedEvent} from './AppThemeTypes';

const NativeAppTheme = NativeModules.RTCAppTheme;

class AppThemeModule extends NativeEventEmitter {
  private _isHighContrast: boolean;
  private _highContrastColors: IHighContrastColors;

  constructor() {
    super(NativeAppTheme);

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

export type AppTheme = AppThemeModule;
export const AppTheme = new AppThemeModule();
