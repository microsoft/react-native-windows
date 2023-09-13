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

import type {ColorValue, NativeColorValue} from './StyleSheet';
import type {ProcessedColorValue} from './processColor';

type LocalNativeColorValue = {
  windowsbrush?: Array<string>,
};

export const PlatformColor = (...names: Array<string>): ColorValue => {
  return ({windowsbrush: names}: LocalNativeColorValue);
};

export const normalizeColorObject = (
  color: NativeColorValue,
): ?ProcessedColorValue => {
  if ('windowsbrush' in (color: LocalNativeColorValue)) {
    return color;
  }
  return null;
};

export const processColorObject = (
  color: NativeColorValue,
): ?NativeColorValue => {
  return color;
};
