/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

export interface IHighContrastColors {
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
  currentTheme: 'light' | 'dark';
}

export interface IHighContrastChangedEvent {
  isHighContrast: boolean;
  highContrastColors: IHighContrastColors;
}
