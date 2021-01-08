/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

export declare type ColorStop = {
  color: string | number | NativeOrDynamicColorType;
  offset: number;
};

// Flexible color type that supports gradients and platform colors.
// Member existence is checked in normalizeColorObject and ensures one set of necessary members is initialized.
export declare type NativeOrDynamicColorType = {
  // Necessary Gradient members
  gradientDirection?: string;
  colorStops?: Array<ColorStop>;
  // Necessary PlatformColor members
  resource_paths?: Array<string>;
};

export declare function PlatformColor(
  ...names: Array<string>
): NativeOrDynamicColorType;
