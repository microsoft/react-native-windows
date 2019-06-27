// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';
import { IAppThemeChangedEvent, IRGBValues } from './AppTheme';

const NativeModules = require('NativeModules');
const ThemingNative = NativeModules.RTCAppTheme;

export class AppThemeModule extends NativeEventEmitter  {
  private _isHighContrast: boolean;
  private _currentTheme: string;
  private _highContrastRGBValues: IRGBValues;

  constructor() {
    super(ThemingNative);

    this._highContrastRGBValues = ThemingNative.highContrastRGBValues;
    /*
    {ButtonFaceRGB: ThemingNative.highContrastRGBValues.ButtonFaceRGB,
                      ButtonTextRGB: ThemingNative.highContrastRGBValues.ButtonTextRGB,
                      GrayTextRGB: ThemingNative.highContrastRGBValues.GrayTextRGB,
                      HighlightRGB: ThemingNative.highContrastRGBValues.HighlightRGB,
                      HighlightTextRGB: ThemingNative.highContrastRGBValues.HighlightTextRGB,
                      HotlightRGB: ThemingNative.highContrastRGBValues.HotlightRGB,
                      WindowRGB: ThemingNative.highContrastRGBValues.WindowRGB,
                      WindowTextRGB: ThemingNative.highContrastRGBValues.WindowTextRGB};*/

    this._isHighContrast = ThemingNative.isHighContrast;
    this.addListener('highContrastChanged', (nativeEvent: IAppThemeChangedEvent) => {
      this._isHighContrast = nativeEvent.isHighContrast;
      this._highContrastRGBValues = nativeEvent.highContrastRGBValues;
    });

    this._currentTheme = ThemingNative.currentTheme;
    this.addListener('appThemeChanged', ({currentTheme}:{currentTheme: string}) => {
      this._currentTheme = currentTheme;
    });
  }

  get isHighContrast(): boolean {
    return this._isHighContrast;
  }

  get currentTheme(): string {
    return this._currentTheme;
  }

  get currentRGBValues(): IRGBValues {
    return this._highContrastRGBValues;
  }
}

export const AppTheme = new AppThemeModule();
export default AppTheme;