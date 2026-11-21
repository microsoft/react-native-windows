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
  type IHighContrastChangedEvent,
  type HighContrastColors,
} from './NativeAppTheme';

// Default values here are used in jest environment, or when native module otherwise not available
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

type AppThemeEvent = (nativeEvent: IHighContrastChangedEvent) => void;
const eventEmitter = new EventEmitter<{
  highContrastChanged: [IHighContrastChangedEvent],
}>();

type NativeAppThemeEventDefinitions = {
  highContrastChanged: [IHighContrastChangedEvent],
};

// $FlowFixMe[underconstrained-implicit-instantiation]
const _notifyHandlers = new Map();

if (NativeAppTheme) {
  _isHighContrast = NativeAppTheme.getConstants().isHighContrast;
  _highContrastColors = NativeAppTheme.getConstants().highContrastColors;

  const nativeEventEmitter =
    new NativeEventEmitter<NativeAppThemeEventDefinitions>(null);
  nativeEventEmitter.addListener(
    'highContrastChanged',
    (newAppTheme: IHighContrastChangedEvent) => {
      _isHighContrast = newAppTheme.isHighContrast;
      _highContrastColors = newAppTheme.highContrastColors;
      eventEmitter.emit('highContrastChanged', newAppTheme);
    },
  );
}

export const AppTheme = {
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
    event: AppThemeEvent,
  ): EventSubscription {
    const listener = eventEmitter.addListener(eventName, event);
    _notifyHandlers.set(eventName, listener);
    return listener;
  },

  removeListener(eventName: 'highContrastChanged', event: AppThemeEvent): void {
    const listener = _notifyHandlers.get(eventName);
    if (!listener) {
      return;
    }
    listener.remove();
    _notifyHandlers.delete(eventName);
  },
};
