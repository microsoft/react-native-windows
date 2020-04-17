/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

'use strict';

export type ColorStop = {
  color: string | number | NativeOrDynamicColorType,
  offset: number,
};

export type NativeOrDynamicColorType = {
  gradientDirection: string,
  colorStops: Array<ColorStop>,
};
