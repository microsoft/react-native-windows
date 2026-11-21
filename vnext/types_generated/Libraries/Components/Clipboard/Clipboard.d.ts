/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<ef5e749188bcea393f3a4c8f80c9bc16>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Clipboard\Clipboard.js
 */

/**
 * `Clipboard` gives you an interface for setting and getting content from Clipboard on both iOS and Android
 *
 * Clipboard has been extracted from react-native core and will be removed in a future release.
 * It can now be installed and imported from `@react-native-clipboard/clipboard` instead of 'react-native'.
 * @see https://github.com/react-native-clipboard/clipboard
 * @deprecated
 */
declare const $$Clipboard: {
  /**
   * Get content of string type, this method returns a `Promise`, so you can use following code to get clipboard content
   * ```javascript
   * async _getContent() {
   *   var content = await Clipboard.getString();
   * }
   * ```
   */
  getString(): Promise<string>;
  /**
   * Set content of string type. You can use following code to set clipboard content
   * ```javascript
   * _setContent() {
   *   Clipboard.setString('hello world');
   * }
   * ```
   * @param {string} content the content to be stored in the clipboard.
   */
  setString(content: string): void;
};
declare type $$Clipboard = typeof $$Clipboard;
export default $$Clipboard;
