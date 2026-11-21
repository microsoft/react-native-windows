/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<9019ca0be634ab0e69c380df86711711>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ActionSheetIOS\ActionSheetIOS.js
 */

import type { ProcessedColorValue } from "../StyleSheet/processColor";
import type { ColorValue } from "../StyleSheet/StyleSheet";
export type ActionSheetIOSOptions = Readonly<{
  title?: string | undefined;
  message?: string | undefined;
  options: Array<string>;
  destructiveButtonIndex?: (number | undefined) | (Array<number> | undefined) | undefined;
  cancelButtonIndex?: number | undefined;
  anchor?: number | undefined;
  tintColor?: ColorValue | ProcessedColorValue | undefined;
  cancelButtonTintColor?: ColorValue | ProcessedColorValue | undefined;
  disabledButtonTintColor?: ColorValue | ProcessedColorValue | undefined;
  userInterfaceStyle?: string | undefined;
  disabledButtonIndices?: Array<number> | undefined;
}>;
export type ShareActionSheetIOSOptions = Readonly<{
  message?: string | undefined;
  url?: string | undefined;
  subject?: string | undefined;
  anchor?: number | undefined;
  tintColor?: number | undefined;
  cancelButtonTintColor?: number | undefined;
  disabledButtonTintColor?: number | undefined;
  /**
   * The activities to exclude from the ActionSheet.
   * For example: ['com.apple.UIKit.activity.PostToTwitter']
   */
  excludedActivityTypes?: Array<string> | undefined;
  userInterfaceStyle?: string | undefined;
}>;
export type ShareActionSheetError = Readonly<{
  domain: string;
  code: string;
  userInfo?: Object | undefined;
  message: string;
}>;
declare const ActionSheetIOS: {
  /**
   * Display an iOS action sheet.
   *
   * The `options` object must contain one or more of:
   *
   * - `options` (array of strings) - a list of button titles (required)
   * - `cancelButtonIndex` (int) - index of cancel button in `options`
   * - `destructiveButtonIndex` (int or array of ints) - indices of destructive buttons in `options`
   * - `title` (string) - a title to show above the action sheet
   * - `message` (string) - a message to show below the title
   * - `disabledButtonIndices` (array of numbers) - a list of button indices which should be disabled
   *
   * The `callback` function receives the zero-based index of the selected
   * item.
   */
  showActionSheetWithOptions(options: ActionSheetIOSOptions, callback: (buttonIndex: number) => void): void;
  /**
   * Display the iOS share sheet. The `options` object should contain one or
   * both of `message` and `url` and can additionally have a `subject` or
   * `excludedActivityTypes`:
   *
   * - `url` (string) - a URL to share
   * - `message` (string) - a message to share
   * - `subject` (string) - a subject for the message
   * - `excludedActivityTypes` (array) - the activities to exclude from the ActionSheet
   * - `tintColor` (color) - tint color of the buttons
   *
   * The `failureCallback` function receives an error object. The
   * `successCallback` function receives a boolean indicating success and a
   * string describing the sharing method used.
   */
  showShareActionSheetWithOptions(options: ShareActionSheetIOSOptions, failureCallback: Function | ((error: ShareActionSheetError) => void), successCallback: Function | ((success: boolean, method: null | undefined | string) => void)): void;
  /**
   * Dismiss the most upper action sheet currently presented. Displays a
   * warning if no action sheet is present.
   */
  dismissActionSheet: () => void;
};
/**
 * Displays native iOS action sheets and share sheets.
 *
 * @see https://reactnative.dev/docs/actionsheetios
 * @platform ios
 */
declare const $$ActionSheetIOS: typeof ActionSheetIOS;
declare type $$ActionSheetIOS = typeof $$ActionSheetIOS;
export default $$ActionSheetIOS;
