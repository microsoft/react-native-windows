/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {
  EmitterSubscription,
  NativeEventEmitter,
  NativeModules,
} from 'react-native';
import {IHighContrastColors, IHighContrastChangedEvent} from './AppThemeTypes';

// We previously gracefully handled importing AppTheme in the Jest environment.
// Mock the NM until we have a coherent story for exporting our own Jest mocks,
// or remove this API.
const NativeAppTheme = NativeModules.RTCAppTheme || {
  initialHighContrast: false,
  initialHighContrastColors: {
    ButtonFaceColor: '',
    ButtonTextColor: '',
    GrayTextColor: '',
    HighlightColor: '',
    HighlightTextColor: '',
    HotlightColor: '',
    WindowColor: '',
    WindowTextColor: '',
  },
};

class AppThemeModule extends NativeEventEmitter {
  private _isHighContrast: boolean;
  private _highContrastColors: IHighContrastColors;

  constructor() {
    super();

    this._highContrastColors = NativeAppTheme.initialHighContrastColors;
    this._isHighContrast = NativeAppTheme.initialHighContrast;
    this.addListener(
      'highContrastChanged',
      (nativeEvent: IHighContrastChangedEvent) => {
        this._isHighContrast = nativeEvent.isHighContrast;
        this._highContrastColors = nativeEvent.highContrastColors;
      },
    );
  }

  addListener(
    eventName: 'highContrastChanged',
    listener: (nativeEvent: IHighContrastChangedEvent) => void,
  ): EmitterSubscription {
    return super.addListener(eventName, listener);
  }

  get isHighContrast(): boolean {
    return this._isHighContrast;
  }

  get currentHighContrastColors(): IHighContrastColors {
    return this._highContrastColors;
  }
}

export type AppTheme = AppThemeModule;
export const AppTheme = new AppThemeModule();
