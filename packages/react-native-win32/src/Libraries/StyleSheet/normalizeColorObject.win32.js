/**
 * Copyright (c) Microsoft Corporation.
 *
 * @format
 * @flow
 */
'use strict';

const invariant = require('invariant');
const normalizeColor = require('./normalizeColor');

import type {NativeOrDynamicColorType} from './NativeOrDynamicColorType';

function normalizeColorObject(
  color: NativeOrDynamicColorType,
): ?NativeOrDynamicColorType {
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

      const gradientColorStops: Array<{
        color: string | number | NativeOrDynamicColorType,
        offset: number,
      }> = [];
      color.colorStops.forEach(
        (colorStop: {
          color: string | number | NativeOrDynamicColorType,
          offset: number,
        }) => {
          gradientColorStops.push({
            // $FlowFixMe
            color: normalizeColor(colorStop.color),
            offset: colorStop.offset,
          });
        },
      );

      return {
        gradientDirection: color.gradientDirection,
        colorStops: gradientColorStops,
      };
    }
  }

  return null;
}

module.exports = normalizeColorObject;
