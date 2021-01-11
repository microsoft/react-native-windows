/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

'use strict';

import type {ColorValue} from './StyleSheet';
import type {ProcessedColorValue} from './processColor';

type GradientColorStop = {|
  color: ColorValue | ProcessedColorValue,
  offset: number,
|};

export type GradientColorValueWin32 = {|
  gradientDirection: 'ToBottom' | 'ToTop' | 'ToRight' | 'ToLeft',
  colorStops: Array<GradientColorStop>,
|};

export opaque type NativeColorValue =
  | $ReadOnly<GradientColorValueWin32>
  | $ReadOnly<{|
      resource_paths?: Array<string>,
    |}>;

export const PlatformColor = (...names: Array<string>): ColorValue => {
  return {resource_paths: names};
};

export const ColorGradientWin32Private = (
  gradientColor: GradientColorValueWin32,
): ColorValue => {
  return gradientColor;
};

export const normalizeColorObject = (
  color: NativeColorValue,
): ?ProcessedColorValue => {
  if ('resource_paths' in color || 'gradientDirection' in color) {
    return color;
  }
  return null;
};

export const processColorObject = (
  color: NativeColorValue,
): ?NativeColorValue => {
  return color;
};
