/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<6727641cf5e6c006cd4bdd3ce96db4d2>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\AppTheme\AppTheme.js
 */

import { type EventSubscription } from "../vendor/emitter/EventEmitter";
import { type IHighContrastChangedEvent, type HighContrastColors } from "./NativeAppTheme";
type AppThemeEvent = (nativeEvent: IHighContrastChangedEvent) => void;
export declare const AppTheme: {
  get isHighContrast(): boolean;
  get currentHighContrastColors(): HighContrastColors;
  /**
   * Add an event handler that is fired when appearance preferences change.
   */
  addListener(eventName: "highContrastChanged", event: AppThemeEvent): EventSubscription;
  removeListener(eventName: "highContrastChanged", event: AppThemeEvent): void;
};
export declare type AppTheme = typeof AppTheme;
