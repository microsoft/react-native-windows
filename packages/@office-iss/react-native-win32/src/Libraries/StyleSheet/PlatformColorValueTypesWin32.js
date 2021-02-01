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
import type {GradientColorValueWin32} from './PlatformColorValueTypes';
import {ColorGradientWin32Private} from './PlatformColorValueTypes';

export const ColorGradientWin32 = (
  gradientColor: GradientColorValueWin32,
): ColorValue => {
  return ColorGradientWin32Private(gradientColor);
};
