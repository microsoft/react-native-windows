// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

export interface RGBValues  {
  ButtonFaceRGB: string;
  ButtonTextRGB: string;
  GrayTextRGB: string;
  HighlightRGB: string;
  HighlightTextRGB: string;
  HotlightRGB: string;
  WindowRGB: string;
  WindowTextRGB: string;
}

export interface IThemingChangedEvent {
  nativeEvent: {
      isHighContrast: boolean;
      RGBValues: RGBValues;
      currentTheme: string;
  };
}