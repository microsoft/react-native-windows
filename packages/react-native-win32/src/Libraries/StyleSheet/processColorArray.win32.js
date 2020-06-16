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

const processColor = require('./processColor');
import type {NativeOrDynamicColorType} from './NativeOrDynamicColorType'; // TODO(macOS ISS#2323203)

function processColorArray(
  /* $FlowFixMe(>=0.89.0 site=react_native_fb) This comment suppresses an
   * error found when Flow v0.89 was deployed. To see the error, delete
   * this comment and run Flow. */
  colors: ?Array<any>,
): ?Array<?(number | NativeOrDynamicColorType)> /* TODO(macOS ISS#2323203) */ {
  // ]TODO(macOS ISS#2323203)
  return colors == null ? null : colors.map(processColor);
}

module.exports = processColorArray;
