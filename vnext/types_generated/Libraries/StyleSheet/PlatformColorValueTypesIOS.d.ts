/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8ce34e9649d473c3d539de513f1e2618>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\StyleSheet\PlatformColorValueTypesIOS.js
 */

import type { ColorValue } from "./StyleSheet";
export type DynamicColorIOSTuple = {
  light: ColorValue;
  dark: ColorValue;
  highContrastLight?: ColorValue | undefined;
  highContrastDark?: ColorValue | undefined;
};
/**
 * Specify color to display depending on the current system appearance settings
 *
 * @param tuple Colors you want to use for "light mode" and "dark mode"
 * @platform ios
 */
export declare const DynamicColorIOS: (tuple: DynamicColorIOSTuple) => ColorValue;
export declare type DynamicColorIOS = typeof DynamicColorIOS;
