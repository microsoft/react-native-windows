/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<843979d0761440a20dadf1a17430b50e>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Share\Share.js
 */

import type { ColorValue } from "../StyleSheet/StyleSheet";
export type ShareContent = {
  title?: string | undefined;
  url: string;
  message?: string | undefined;
} | {
  title?: string | undefined;
  url?: string | undefined;
  message: string;
};
export type ShareOptions = {
  dialogTitle?: string | undefined;
  excludedActivityTypes?: Array<string> | undefined;
  tintColor?: ColorValue | undefined;
  subject?: string | undefined;
  anchor?: number | undefined;
};
export type ShareAction = {
  action: "sharedAction" | "dismissedAction";
  activityType?: string | null | undefined;
};
/**
 * Opens a dialog to share text content.
 *
 * @see https://reactnative.dev/docs/share
 */
declare class Share {
  /**
   * Open a share dialog to share text content.
   *
   * On iOS, returns a Promise which resolves to an object containing `action`
   * and `activityType`. If the user dismissed the dialog, the Promise will
   * still resolve with action being `Share.dismissedAction` and all the other
   * keys being undefined.
   *
   * On Android, returns a Promise which always resolves with action being
   * `Share.sharedAction`.
   *
   * **Content:**
   *
   * - `message` - A message to share.
   * - `url` (iOS) - A URL to share.
   * - `title` (Android) - Title of the message.
   *
   * At least one of `url` or `message` is required.
   *
   * **Options:**
   *
   * - `dialogTitle` (Android) - Title of the share dialog.
   * - `excludedActivityTypes` (iOS) - Activity types to exclude.
   * - `subject` (iOS) - A subject to share via email.
   * - `tintColor` (iOS) - Tint color for the share dialog.
   * - `anchor` (iOS) - The anchor point for the popover (iPad).
   */
  static share(content: ShareContent, options?: ShareOptions): Promise<{
    action: string;
    activityType: string | undefined;
  }>;
  /**
   * The content was successfully shared.
   */
  static sharedAction: "sharedAction";
  /**
   * The dialog was dismissed.
   *
   * @platform ios
   */
  static dismissedAction: "dismissedAction";
}
export default Share;
