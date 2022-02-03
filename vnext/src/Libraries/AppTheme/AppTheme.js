/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict-local
 */

import EventEmitter, {
  type EventSubscription,
} from '../vendor/emitter/EventEmitter';
import NativeEventEmitter from '../EventEmitter/NativeEventEmitter';
import NativeAppTheme, {
  type AppThemeData,
  type HighContrastColors,
} from './NativeAppTheme';

// Default values here are used in jest environment, or when native module otherwise not availiable
let _isHighContrast = false;
let _highContrastColors = {
  ButtonFaceColor: '',
  ButtonTextColor: '',
  GrayTextColor: '',
  HighlightColor: '',
  HighlightTextColor: '',
  HotlightColor: '',
  WindowColor: '',
  WindowTextColor: '',
};

type AppThemeListener = (nativeEvent: AppThemeData) => void;
const eventEmitter = new EventEmitter<{
  highContrastChanged: [AppThemeData],
}>();

type NativeAppThemeEventDefinitions = {
  highContrastChanged: [AppThemeData],
};

if (NativeAppTheme) {
  _isHighContrast = NativeAppTheme.getConstants().isHighContrast;
  _highContrastColors = NativeAppTheme.getConstants().highContrastColors;

  const nativeEventEmitter =
    new NativeEventEmitter<NativeAppThemeEventDefinitions>(null);
  nativeEventEmitter.addListener(
    'highContrastChanged',
    (newAppTheme: AppThemeData) => {
      _isHighContrast = newAppTheme.isHighContrast;
      _highContrastColors = newAppTheme.highContrastColors;
      eventEmitter.emit('highContrastChanged', newAppTheme);
    },
  );
}

module.exports = {
  // $FlowFixMe[unsafe-getters-setters]
  get isHighContrast(): boolean {
    return _isHighContrast;
  },

  // $FlowFixMe[unsafe-getters-setters]
  get currentHighContrastColors(): HighContrastColors {
    return _highContrastColors;
  },

  /**
   * Add an event handler that is fired when appearance preferences change.
   */
  addListener(
    eventName: 'highContrastChanged',
    listener: AppThemeListener,
  ): EventSubscription {
    return eventEmitter.addListener(eventName, listener);
  },

  removeListener(
    eventName: 'highContrastChanged',
    listener: AppThemeListener,
  ): void {
    eventEmitter.removeListener(eventName, listener);
  },
};
