/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {EmitterSubscription} from 'react-native';

/**
 * Color information for high contrast
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

export interface IHighContrastChangedEvent {
  isHighContrast: boolean;
  highContrastColors: IHighContrastColors;
}

export type AppTheme = {
  addListener(
    eventName: 'highContrastChanged',
    listener: (nativeEvent: IHighContrastChangedEvent) => void,
  ): EmitterSubscription;

  removeListener(
    eventName: 'highContrastChanged',
    listener: (nativeEvent: IHighContrastChangedEvent) => void,
  ): void;

  isHighContrast: boolean;

  currentHighContrastColors: IHighContrastColors;
};

export const AppTheme: AppTheme;
