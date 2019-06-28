// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter, NativeModules } from 'react-native';
const MissingNativeEventEmitterShim = require('MissingNativeEventEmitterShim');
import { IAppThemeChangedEvent, IRGBValues } from './AppTheme';

const NativeAppTheme = NativeModules.RTCAppTheme;

class AppThemeModule extends NativeEventEmitter  {
  public isAvailable: boolean;
  private _isHighContrast: boolean;
  private _currentTheme: string;
  private _highContrastRGBValues: IRGBValues;

  constructor() {
    super(NativeAppTheme);
    this.isAvailable = true;

    this._highContrastRGBValues = NativeAppTheme.highContrastRGBValues;
    /*
    {ButtonFaceRGB: ThemingNative.highContrastRGBValues.ButtonFaceRGB,
                      ButtonTextRGB: ThemingNative.highContrastRGBValues.ButtonTextRGB,
                      GrayTextRGB: ThemingNative.highContrastRGBValues.GrayTextRGB,
                      HighlightRGB: ThemingNative.highContrastRGBValues.HighlightRGB,
                      HighlightTextRGB: ThemingNative.highContrastRGBValues.HighlightTextRGB,
                      HotlightRGB: ThemingNative.highContrastRGBValues.HotlightRGB,
                      WindowRGB: ThemingNative.highContrastRGBValues.WindowRGB,
                      WindowTextRGB: ThemingNative.highContrastRGBValues.WindowTextRGB};*/

    this._isHighContrast = NativeAppTheme.isHighContrast;
    this.addListener('highContrastChanged', (nativeEvent: IAppThemeChangedEvent) => {
      this._isHighContrast = nativeEvent.isHighContrast;
      this._highContrastRGBValues = nativeEvent.highContrastRGBValues;
    });

    this._currentTheme = NativeAppTheme.initialAppTheme;
    this.addListener('appThemeChanged', ({currentTheme}:{currentTheme: string}) => {
      this._currentTheme = currentTheme;
    });
  }

  get currentTheme(): string {
    return this._currentTheme;
  }

  get isHighContrast(): boolean {
    return this._isHighContrast;
  }

  get currentRGBValues(): IRGBValues {
    return this._highContrastRGBValues;
  }
}

// This module depends on the native `RCTAppTheme` module. If you don't include it,
// `AppTheme.isAvailable` will return `false`, and any method calls will throw.
class MissingNativeAppThemeShim extends MissingNativeEventEmitterShim {
  public isAvailable = false;
  public currentTheme = '';
}

export const AppTheme = (NativeAppTheme ? new AppThemeModule() : new MissingNativeAppThemeShim());
export default AppTheme;