/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<c65d5e1c4d41ef59428895b9b30bd8c5>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Keyboard\Keyboard.js
 */

import type { EventSubscription } from "../../vendor/emitter/EventEmitter";
export type KeyboardEventName = keyof KeyboardEventDefinitions;
export type KeyboardEventEasing = "easeIn" | "easeInEaseOut" | "easeOut" | "linear" | "keyboard";
export type KeyboardMetrics = Readonly<{
  screenX: number;
  screenY: number;
  width: number;
  height: number;
}>;
export type KeyboardEvent = AndroidKeyboardEvent | IOSKeyboardEvent;
type BaseKeyboardEvent = {
  duration: number;
  easing: KeyboardEventEasing;
  endCoordinates: KeyboardMetrics;
};
export type AndroidKeyboardEvent = Readonly<Omit<BaseKeyboardEvent, "duration" | "easing"> & {
  /**
   * Always set to 0 on Android.
   */
  duration: 0;
  /**
   * Always set to "keyboard" on Android.
   */
  easing: "keyboard";
}>;
export type IOSKeyboardEvent = Readonly<Omit<BaseKeyboardEvent, "startCoordinates" | "isEventFromThisApp"> & {
  startCoordinates: KeyboardMetrics;
  isEventFromThisApp: boolean;
}>;
type KeyboardEventDefinitions = {
  keyboardWillShow: [KeyboardEvent];
  keyboardDidShow: [KeyboardEvent];
  keyboardWillHide: [KeyboardEvent];
  keyboardDidHide: [KeyboardEvent];
  keyboardWillChangeFrame: [KeyboardEvent];
  keyboardDidChangeFrame: [KeyboardEvent];
};
/**
 * Module to control keyboard events and make changes to the keyboard.
 */
declare class KeyboardImpl {
  constructor();
  /**
   * Listen for native keyboard notification events.
   *
   * Available events are: `keyboardWillShow`, `keyboardDidShow`,
   * `keyboardWillHide`, `keyboardDidHide`, `keyboardWillChangeFrame`, and
   * `keyboardDidChangeFrame`. Only `keyboardDidShow` and `keyboardDidHide`
   * are available on Android.
   *
   * Android versions prior to API 30 rely on observing layout changes when
   * `android:windowSoftInputMode` is set to `adjustResize` or `adjustPan`.
   *
   * @param {string} eventName The native event name to listen for.
   * @param {function} callback Function to be called when the event fires.
   */
  addListener<K extends keyof KeyboardEventDefinitions>(eventType: K, listener: (...$$REST$$: KeyboardEventDefinitions[K]) => unknown, context?: unknown): EventSubscription;
  /**
   * Removes all listeners for a specific event type.
   *
   * @param {string} eventType The native event string listeners are watching which will be removed.
   */
  removeAllListeners<K extends keyof KeyboardEventDefinitions>(eventType: null | undefined | K): void;
  /**
   * Dismisses the active keyboard and removes focus.
   */
  dismiss(): void;
  /**
   * Whether the keyboard is last known to be visible.
   */
  isVisible(): boolean;
  /**
   * Return the metrics of the soft keyboard if visible.
   */
  metrics(): null | undefined | KeyboardMetrics;
  /**
   * Sync `TextInput` (or other keyboard accessory view) size or position
   * changes with keyboard movements.
   */
  scheduleLayoutAnimation(event: KeyboardEvent): void;
}
declare const Keyboard: KeyboardImpl;
/**
 * `Keyboard` module to control keyboard events and make changes to the
 * keyboard, like dismissing it.
 *
 * @see https://reactnative.dev/docs/keyboard
 */
declare const $$Keyboard: typeof Keyboard;
declare type $$Keyboard = typeof $$Keyboard;
export default $$Keyboard;
