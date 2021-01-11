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

export opaque type NativeColorValue = {
  windowsbrush?: string,
};

export const PlatformColor = (...names: Array<string>): ColorValue => {
  // We dont support fallback colors right now, so no point in sending more than the first color across the bridge
  return {windowsbrush: names[0]};
};

export const normalizeColorObject = (
  color: NativeColorValue,
): ?ProcessedColorValue => {
  if ('windowsbrush' in color) {
    return color;
  }
  return null;
};

export const processColorObject = (
  color: NativeColorValue,
): ?NativeColorValue => {
  return color;
};
