// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';
import { IThemingChangedEvent, RGBValues } from './Theming';

const NativeModules = require('NativeModules');
const ThemingNative = NativeModules.ThemingModule;

export class Theming extends NativeEventEmitter  {
  private _isHighContrast: boolean;
  private _currentTheme: string;
  private _highContrastRGBValues: RGBValues;

  constructor() {
    super(ThemingNative);

    this._highContrastRGBValues = {ButtonFaceRGB: ThemingNative.highContrastRGBValues.ButtonFaceRGB,
                      ButtonTextRGB: ThemingNative.highContrastRGBValues.ButtonTextRGB,
                      GrayTextRGB: ThemingNative.highContrastRGBValues.GrayTextRGB,
                      HighlightRGB: ThemingNative.highContrastRGBValues.HighlightRGB,
                      HighlightTextRGB: ThemingNative.highContrastRGBValues.HighlightTextRGB,
                      HotlightRGB: ThemingNative.highContrastRGBValues.HotlightRGB,
                      WindowRGB: ThemingNative.highContrastRGBValues.WindowRGB,
                      WindowTextRGB: ThemingNative.highContrastRGBValues.WindowTextRGB};

    this._isHighContrast = ThemingNative.isHighContrast;
    this.addListener('highContrastChanged', (eventData: IThemingChangedEvent) => {
      this._isHighContrast = eventData.nativeEvent.isHighContrast;
      this._highContrastRGBValues = eventData.nativeEvent.RGBValues;
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

  get RGBValues(): RGBValues {
    return this._highContrastRGBValues;
  }
}

export default Theming;