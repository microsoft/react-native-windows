// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

export interface IRGBValues  {
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
    isHighContrast: boolean;
    highContrastRGBValues: IRGBValues;
    currentTheme: string;
}