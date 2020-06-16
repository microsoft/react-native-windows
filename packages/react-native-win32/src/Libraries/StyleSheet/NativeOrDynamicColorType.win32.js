/**
 * Copyright (c) Microsoft Corporation.
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
