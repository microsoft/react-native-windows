// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';

const NativeModules = require('NativeModules');
const ThemingNative = NativeModules.RTCAppTheme;

class ThemingModule extends NativeEventEmitter  {
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

export const Theming = new ThemingModule();
export default Theming;