/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

import type {ColorValue, NativeColorValue} from './StyleSheet';
import type {ProcessedColorValue} from './processColor';

type GradientColorStop = {|
  color: ColorValue | ProcessedColorValue,
  offset: number,
|};

export type GradientColorValueWin32 = {|
  gradientDirection: 'ToBottom' | 'ToTop' | 'ToRight' | 'ToLeft',
  colorStops: Array<GradientColorStop>,
|};

/** The actual type of the opaque NativeColorValue on Windows platform */
type LocalNativeColorValue =
  | $ReadOnly<GradientColorValueWin32>
  | $ReadOnly<{|
      resource_paths?: Array<string>,
    |}>;

export const PlatformColor = (...names: Array<string>): ColorValue => {
  // $FlowExpectedError[incompatible-return] LocalNativeColorValue is the Windows LocalNativeColorValue type
  // $FlowExpectedError[incompatible-type]
  return ({resource_paths: names}: LocalNativeColorValue);
};

export const ColorGradientWin32Private = (
  gradientColor: GradientColorValueWin32,
): ColorValue => {
  // $FlowExpectedError[incompatible-return]
  // $FlowExpectedError[incompatible-type]
  return gradientColor;
};

export const normalizeColorObject = (
  color: NativeColorValue,
): ?ProcessedColorValue => {
  /* $FlowExpectedError[incompatible-cast]
   * LocalNativeColorValue is the actual type of the opaque NativeColorValue on Windows platform */
  if (
    // $FlowExpectedError[incompatible-cast]
    // $FlowExpectedError[incompatible-type]
    'resource_paths' in (color: LocalNativeColorValue) ||
    // $FlowExpectedError[incompatible-cast]
    // $FlowExpectedError[incompatible-type]
    'gradientDirection' in (color: LocalNativeColorValue)
  ) {
    return color;
  }
  return null;
};

export const processColorObject = (
  color: NativeColorValue,
): ?NativeColorValue => {
  return color;
};
