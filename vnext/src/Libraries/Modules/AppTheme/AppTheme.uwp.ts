// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter, NativeModules } from 'react-native';

const ThemingNative = NativeModules.RTCAppTheme;

class AppThemeModule extends NativeEventEmitter  {
  private _currentTheme: string;

  constructor() {
    super(ThemingNative);

    this._currentTheme = ThemingNative.initialAppTheme;
    this.addListener('appThemeChanged', ({currentTheme}:{currentTheme: string}) => {
      this._currentTheme = currentTheme;
    });
  }

  get currentTheme(): string {
    return this._currentTheme;
  }
}

export const AppTheme = new AppThemeModule();
export default AppTheme;