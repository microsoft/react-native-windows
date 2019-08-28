/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License
 * @format
 */

'use strict';

export type NativeOrDynamicColorType = {
  windowsbrush?: string,
};

function normalizeColorObject(
  color: NativeOrDynamicColorType,
): ?(number | NativeOrDynamicColorType) {
  if ('windowsbrush' in color) {
    return color;
  }

  return null;
}

module.exports = normalizeColorObject;
