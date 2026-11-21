/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<630a06fcc05435548820e1f473a50c7b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\I18nManager.js
 */

import type { I18nManagerConstants } from "./NativeI18nManager";
/**
 * Utilities for managing Right-to-Left (RTL) layout support.
 *
 * @see https://reactnative.dev/docs/i18nmanager
 */
declare const $$I18nManager: {
  getConstants: () => I18nManagerConstants;
  /**
   * Allows the app to opt in to RTL layout behavior. Should be called early
   * in app startup. Takes effect on the next app reload.
   */
  allowRTL: (shouldAllow: boolean) => void;
  /**
   * Forces the app into RTL layout mode, regardless of the device locale.
   * Takes effect on the next app reload.
   */
  forceRTL: (shouldForce: boolean) => void;
  /**
   * Controls whether `left`/`right` style properties are automatically swapped
   * in RTL layouts. When enabled, `left` becomes `right` and vice versa.
   */
  swapLeftAndRightInRTL: (flipStyles: boolean) => void;
  /** Whether the current layout direction is Right-to-Left. */
  isRTL: I18nManagerConstants["isRTL"];
  /** Whether left and right style properties are swapped in RTL mode. */
  doLeftAndRightSwapInRTL: I18nManagerConstants["doLeftAndRightSwapInRTL"];
};
declare type $$I18nManager = typeof $$I18nManager;
export default $$I18nManager;
