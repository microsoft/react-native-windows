/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<5c4f42e761da4626580f38b8dfcf8da4>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Core\ReactNativeVersion.js
 */

/**
 * Object containing the current React Native version.
 *
 * Specifically, this is the source of truth for the resolved `react-native`
 * package in the JavaScript bundle. Apps and libraries can use this to
 * determine compatibility or enable version-specific features.
 *
 * @example
 * ```js
 * // Get the full version string
 * const version = ReactNativeVersion.getVersionString();
 *
 * // Access individual version components
 * const major = ReactNativeVersion.major;
 * ```
 */
declare class ReactNativeVersion {
  static major: number;
  static minor: number;
  static patch: number;
  static prerelease: string | null;
  static getVersionString(): string;
}
export default ReactNativeVersion;
/**
 * @deprecated Compatibility export — please import `ReactNativeVersion` from
 *   `react-native`.
 * See https://github.com/react-native-community/discussions-and-proposals/pull/894.
 */
export declare const version: {
  major: typeof ReactNativeVersion.major;
  minor: typeof ReactNativeVersion.minor;
  patch: typeof ReactNativeVersion.patch;
  prerelease: typeof ReactNativeVersion.prerelease;
};
export declare type version = typeof version;
