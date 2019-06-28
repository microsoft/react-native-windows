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
  currentTheme: string;
}

export interface IHighContrastChangedEvent {
  isHighContrast: boolean;
  highContrastColors: IColorValues;
}