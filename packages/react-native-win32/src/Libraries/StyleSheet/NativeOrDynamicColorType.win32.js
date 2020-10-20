/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

'use strict';

const invariant = require('invariant');

export type ColorStop = {
  color: string | number | NativeOrDynamicColorType,
  offset: number,
};

// Flexible color type that supports gradients and platform colors.
// Member existence is checked in normalizeColorObject and ensures one set of necessary members is initialized.
export type NativeOrDynamicColorType = {
  // Necessary Gradient members
  gradientDirection?: string,
  colorStops?: Array<ColorStop>,

  // Necessary PlatformColor members
  resource_paths?: Array<string>,
};

export function PlatformColor(
  ...names: Array<string>
): NativeOrDynamicColorType {
  return {resource_paths: names};
}

// Generalized color handling function for re-entrant color processing of color objects with nested colors.
export function handleColorObject(
  color?: NativeOrDynamicColorType,
  handleColor: (
    color: string | number | NativeOrDynamicColorType,
  ) => ?(number | NativeOrDynamicColorType),
): ?NativeOrDynamicColorType {
  if (color && typeof color === 'object') {
    if (
      color.hasOwnProperty('colorStops') &&
      color.hasOwnProperty('gradientDirection') &&
      !color.hasOwnProperty('resource_paths')
    ) {
      invariant(
        color.colorStops.length >= 2,
        'Gradients must contain at least two colors.',
      );

      invariant(
        color.gradientDirection === 'ToTop' ||
          color.gradientDirection === 'ToBottom' ||
          color.gradientDirection === 'ToLeft' ||
          color.gradientDirection === 'ToRight',
        'Unsupported gradient direction; currently supports ToTop, ToBottom, ToLeft, and ToRight.',
      );

      const gradientColorStops: Array<ColorStop> = [];
      color.colorStops.forEach((colorStop: ColorStop) => {
        gradientColorStops.push({
          // $FlowFixMe
          color: handleColor(colorStop.color),
          offset: colorStop.offset,
        });
      });

      return {
        gradientDirection: color.gradientDirection,
        colorStops: gradientColorStops,
      };
    } else if (
      color.hasOwnProperty('resource_paths') &&
      !color.hasOwnProperty('colorStops') &&
      !color.hasOwnProperty('gradientDirection')
    ) {
      return color;
    }
  }

  return null;
}
