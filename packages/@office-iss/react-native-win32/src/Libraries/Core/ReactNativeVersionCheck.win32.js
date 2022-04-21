/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict
 * @format
 */

import Platform from '../Utilities/Platform';
const ReactNativeVersion = require('./ReactNativeVersion');

/**
 * Checks that the version of this React Native JS is compatible with the native
 * code, throwing an error if it isn't.
 *
 * The existence of this module is part of the public interface of React Native
 * even though it is used only internally within React Native. React Native
 * implementations for other platforms (ex: Windows) may override this module
 * and rely on its existence as a separate module.
 */
exports.checkVersions = function checkVersions(): void {
  const nativeVersion = Platform.constants.reactNativeVersion;
  if (
    ReactNativeVersion.version.major !== nativeVersion.major ||
    ReactNativeVersion.version.minor !== nativeVersion.minor
  ) {
    // [Win32 We cannot always match versions. Warn instead of error
    console.warn(
      // Win32]
      `React Native version mismatch.\n\nJavaScript version: ${_formatVersion(
        ReactNativeVersion.version,
      )}\n` +
        `Native version: ${_formatVersion(nativeVersion)}\n\n` +
        'Make sure that you have rebuilt the native code. If the problem ' +
        'persists try clearing the Watchman and packager caches with ' +
        '`watchman watch-del-all && react-native start --reset-cache`.',
    );
  }
};

function _formatVersion(
  version:
    | {major: number, minor: number, patch: number, prerelease: ?number}
    | {major: number, minor: number, patch: number, prerelease: ?string}
    | $TEMPORARY$object<{
        major: number,
        minor: number,
        patch: number,
        prerelease: null,
      }>,
): string {
  return (
    `${version.major}.${version.minor}.${version.patch}` +
    // eslint-disable-next-line eqeqeq
    (version.prerelease != undefined ? `-${version.prerelease}` : '')
  );
}
