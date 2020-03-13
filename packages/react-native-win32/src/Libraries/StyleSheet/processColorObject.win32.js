/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 *
 * @format
 * @flow
 */
'use strict';

const invariant = require('invariant');
const processColor = require('./processColor');

import type {
  ColorStop,
  NativeOrDynamicColorType,
} from './NativeOrDynamicColorType';

function processColorObject(color?: NativeOrDynamicColorType) {
  if (color && typeof color === 'object') {
    if (
      color.hasOwnProperty('colorStops') &&
      color.hasOwnProperty('gradientDirection')
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
          //$FlowFixMe
          color: processColor(colorStop.color),
          offset: colorStop.offset,
        });
      });

      return {
        gradientDirection: color.gradientDirection,
        colorStops: gradientColorStops,
      };
    }
  }

  return null;
}

module.exports = processColorObject;
// ]TODO(macOS ISS#2323203)
