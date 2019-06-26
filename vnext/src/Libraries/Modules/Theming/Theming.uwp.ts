// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { NativeEventEmitter } from 'react-native';

const NativeModules = require('NativeModules');
const ThemingNative = NativeModules.ThemingModule;

export class Theming extends NativeEventEmitter  {
  private misHighContrast: boolean;
  private mcurrentTheme: string;
  private mRGBValues = ["", "", "", "", "", "", "", ""];

  constructor() {
    super(ThemingNative);

    this.mRGBValues[0] = ThemingNative.highContrastRGBValues.ButtonFaceRGB;
    this.mRGBValues[1] = ThemingNative.highContrastRGBValues.ButtonTextRGB;
    this.mRGBValues[2] = ThemingNative.highContrastRGBValues.GrayTextRGB;
    this.mRGBValues[3] = ThemingNative.highContrastRGBValues.HighlightRGB;
    this.mRGBValues[4] = ThemingNative.highContrastRGBValues.HighlightTextRGB;
    this.mRGBValues[5] = ThemingNative.highContrastRGBValues.HotlightRGB;
    this.mRGBValues[6] = ThemingNative.highContrastRGBValues.WindowRGB;
    this.mRGBValues[7] = ThemingNative.highContrastRGBValues.WindowTextRGB;

    this.misHighContrast = ThemingNative.isHighContrast;
    this.addListener('highContrastDidChange', (args): any => {
      this.misHighContrast = args.highContrastRGBValues !== 'None';
    });

    this.mcurrentTheme = ThemingNative.currentTheme;
    this.addListener('themeDidChange', (args): any => {
      this.mcurrentTheme = args.platform_theme;
    });
  }

  get isHighContrast(): boolean {
    return this.misHighContrast;
  }

  get currentTheme(): string {
    return this.mcurrentTheme;
  }

  get RGBValues(): string[] {
    return this.mRGBValues;
  }
}

export default Theming;