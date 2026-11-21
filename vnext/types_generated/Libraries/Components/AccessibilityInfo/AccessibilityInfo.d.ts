/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<7c952a05a312f6ea42f2ff0604204644>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\AccessibilityInfo\AccessibilityInfo.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { EventSubscription } from "../../vendor/emitter/EventEmitter";
type AccessibilityEventDefinitionsAndroid = {
  accessibilityServiceChanged: [boolean];
  highTextContrastChanged: [boolean];
};
type AccessibilityEventDefinitionsIOS = {
  announcementFinished: [{
    announcement: string;
    success: boolean;
  }];
  boldTextChanged: [boolean];
  darkerSystemColorsChanged: [boolean];
  reduceTransparencyChanged: [boolean];
  showBordersChanged: [boolean];
};
type AccessibilityEventDefinitions = Omit<AccessibilityEventDefinitionsAndroid, keyof AccessibilityEventDefinitionsIOS | "change" | "grayscaleChanged" | "invertColorsChanged" | "reduceMotionChanged" | "screenReaderChanged"> & Omit<AccessibilityEventDefinitionsIOS, "change" | "grayscaleChanged" | "invertColorsChanged" | "reduceMotionChanged" | "screenReaderChanged"> & {
  change: [boolean];
  grayscaleChanged: [boolean];
  invertColorsChanged: [boolean];
  reduceMotionChanged: [boolean];
  screenReaderChanged: [boolean];
};
type AccessibilityEventTypes = "click" | "focus" | "viewHoverEnter" | "windowStateChange";
declare const AccessibilityInfo: {
  /**
   * Query whether bold text is currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when bold text is enabled and `false` otherwise.
   *
   * @platform ios
   */
  isBoldTextEnabled(): Promise<boolean>;
  /**
   * Query whether grayscale is currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when grayscale is enabled and `false` otherwise.
   *
   * @platform ios
   */
  isGrayscaleEnabled(): Promise<boolean>;
  /**
   * Query whether inverted colors are currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when invert color is enabled and `false` otherwise.
   *
   * @platform ios
   */
  isInvertColorsEnabled(): Promise<boolean>;
  /**
   * Query whether reduced motion is currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when a reduce motion is enabled and `false` otherwise.
   */
  isReduceMotionEnabled(): Promise<boolean>;
  /**
   * Query whether high text contrast is currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when high text contrast is enabled and `false` otherwise.
   *
   * @platform android
   */
  isHighTextContrastEnabled(): Promise<boolean>;
  /**
   * Query whether darker system colors is currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when darker system colors is enabled and `false` otherwise.
   *
   * @platform ios
   */
  isDarkerSystemColorsEnabled(): Promise<boolean>;
  /**
   * Query whether cross-fade transitions are preferred over slide transitions.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when cross-fade transitions are preferred and `false` otherwise.
   *
   * @platform ios
   */
  prefersCrossFadeTransitions(): Promise<boolean>;
  /**
   * Query whether Show Borders (called Button Shapes before iOS 26.1) is
   * currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when show borders is enabled and `false` otherwise.
   *
   * @platform ios
   */
  isShowBordersEnabled(): Promise<boolean>;
  /**
   * Query whether reduced transparency is currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when a reduce transparency is enabled and `false` otherwise.
   *
   * @platform ios
   */
  isReduceTransparencyEnabled(): Promise<boolean>;
  /**
   * Query whether a screen reader is currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when a screen reader is enabled and `false` otherwise.
   */
  isScreenReaderEnabled(): Promise<boolean>;
  /**
   * Query whether Accessibility Service is currently enabled.
   *
   * Returns a promise which resolves to a boolean.
   * The result is `true` when any service is enabled and `false` otherwise.
   *
   * @platform android
   */
  isAccessibilityServiceEnabled(): Promise<boolean>;
  /**
   * Add an event handler. Supported events:
   *
   * - `reduceMotionChanged`: Fires when the state of the reduce motion toggle changes.
   *   The argument to the event handler is a boolean. The boolean is `true` when a reduce
   *   motion is enabled (or when "Transition Animation Scale" in "Developer options" is
   *   "Animation off") and `false` otherwise.
   * - `screenReaderChanged`: Fires when the state of the screen reader changes. The argument
   *   to the event handler is a boolean. The boolean is `true` when a screen
   *   reader is enabled and `false` otherwise.
   * - `grayscaleChanged`: Fires when the state of the gray scale toggle changes.
   *   The argument to the event handler is a boolean. The boolean is `true` when gray scale
   *   is enabled and `false` otherwise.
   * - `invertColorsChanged`: Fires when the state of the invert colors toggle
   *   changes. The argument to the event handler is a boolean. The boolean is `true` when invert
   *   colors is enabled and `false` otherwise.
   *
   * These events are only supported on iOS:
   *
   * - `boldTextChanged`: iOS-only event. Fires when the state of the bold text toggle changes.
   *   The argument to the event handler is a boolean. The boolean is `true` when a bold text
   *   is enabled and `false` otherwise.
   * - `reduceTransparencyChanged`: iOS-only event. Fires when the state of the reduce transparency
   *   toggle changes.  The argument to the event handler is a boolean. The boolean is `true`
   *   when a reduce transparency is enabled and `false` otherwise.
   * - `announcementFinished`: iOS-only event. Fires when the screen reader has
   *   finished making an announcement. The argument to the event handler is a
   *   dictionary with these keys:
   *     - `announcement`: The string announced by the screen reader.
   *     - `success`: A boolean indicating whether the announcement was
   *       successfully made.
   * - `darkerSystemColorsChanged`: iOS-only event. Fires when the state of the dark system colors
   *   toggle changes. The argument to the event handler is a boolean. The boolean is `true` when
   *   dark system colors is enabled and `false` otherwise.
   *
   * - `showBordersChanged`: iOS-only event. Fires when the state of the show borders toggle changes
   *   (called Button Shapes before iOS 26.1).
   *   The argument to the event handler is a boolean.
   *
   * These events are only supported on Android:
   *
   * - `highTextContrastChanged`: Android-only event. Fires when the state of the high text contrast
   *   toggle changes. The argument to the event handler is a boolean. The boolean is `true` when
   *   high text contrast is enabled and `false` otherwise.
   */
  addEventListener<K extends keyof AccessibilityEventDefinitions>(eventName: K, handler: (...$$REST$$: AccessibilityEventDefinitions[K]) => void): EventSubscription;
  /**
   * Set accessibility focus to a React component.
   *
   * @deprecated Use `sendAccessibilityEvent` with eventType `focus` instead.
   */
  setAccessibilityFocus(reactTag: number): void;
  /**
   * Trigger an accessibility event on a host instance. The `eventType` can be
   * `'focus'`, `'click'`, `'viewHoverEnter'`, or `'windowStateChange'`.
   */
  sendAccessibilityEvent(handle: HostInstance, eventType: AccessibilityEventTypes): void;
  /**
   * Post a string to be announced by the screen reader.
   */
  announceForAccessibility(announcement: string): void;
  /**
   * Post a string to be announced by the screen reader with options.
   *
   * - `announcement`: The string announced by the screen reader.
   * - `options`: An object that configures the reading options.
   *   - `queue`: The announcement will be queued behind existing announcements. iOS only.
   *   - `priority`: The priority of the announcement. Possible values: 'low' | 'default' | 'high'.
   *     High priority announcements will interrupt any ongoing speech and cannot be interrupted.
   *     Default priority announcements will interrupt any ongoing speech but can be interrupted.
   *     Low priority announcements will not interrupt ongoing speech and can be interrupted.
   *     (iOS only).
   */
  announceForAccessibilityWithOptions(announcement: string, options: {
    queue?: boolean | undefined;
    priority?: "low" | "default" | "high" | undefined;
  }): void;
  /**
   * Get the recommended timeout in milliseconds the user needs, as specified
   * in the device's Accessibility settings.
   *
   * @platform android
   */
  getRecommendedTimeoutMillis(originalTimeout: number): Promise<number>;
};
/**
 * Provides information about the device's accessibility features, such as whether a screen reader is active. Can be used to query the current state and register for change notifications.
 *
 * @see https://reactnative.dev/docs/accessibilityinfo
 */
declare const $$AccessibilityInfo: typeof AccessibilityInfo;
declare type $$AccessibilityInfo = typeof $$AccessibilityInfo;
export default $$AccessibilityInfo;
