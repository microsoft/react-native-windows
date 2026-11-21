/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<dbb2c16edf3cb7bdadeeb3554288c946>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\StyleSheet\processColor.js
 */

import type { ColorValue, NativeColorValue } from "./StyleSheet";
export type ProcessedColorValue = number | NativeColorValue;
declare function processColor(color?: null | undefined | (number | ColorValue)): null | undefined | ProcessedColorValue;
declare const $$processColor: typeof processColor;
declare type $$processColor = typeof $$processColor;
export default $$processColor;
