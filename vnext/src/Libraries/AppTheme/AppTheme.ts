// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

export interface IColorValues  {
  ButtonFaceColor: string;
  ButtonTextColor: string;
  GrayTextColor: string;
  HighlightColor: string;
  HighlightTextColor: string;
  HotlightColor: string;
  WindowColor: string;
  WindowTextColor: string;
}

export interface IAppThemeChangedEvent {
    isHighContrast: boolean;
    highContrastColors: IColorValues;
    currentTheme: string;
}