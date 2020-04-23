/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

export type NativeOrDynamicColorType = {};

function normalizeColorObject(
  color: NativeOrDynamicColorType,
): ?(number | NativeOrDynamicColorType) {
  return null;
}

module.exports = normalizeColorObject;
