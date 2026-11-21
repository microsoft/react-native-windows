/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<535684cc300fed2b9c37a00ff944fcad>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\nodes\AnimatedColor.js
 */

import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { NativeColorValue } from "../../StyleSheet/StyleSheetTypes";
import type { AnimatedNodeConfig } from "./AnimatedNode";
import AnimatedValue from "./AnimatedValue";
import AnimatedWithChildren from "./AnimatedWithChildren";
export type AnimatedColorConfig = Readonly<Omit<AnimatedNodeConfig, "useNativeDriver"> & {
  useNativeDriver: boolean;
}>;
type ColorListenerCallback = (value: ColorValue) => unknown;
export type RgbaValue = {
  readonly r: number;
  readonly g: number;
  readonly b: number;
  readonly a: number;
};
type RgbaAnimatedValue = {
  readonly r: AnimatedValue;
  readonly g: AnimatedValue;
  readonly b: AnimatedValue;
  readonly a: AnimatedValue;
};
export type InputValue = null | undefined | (RgbaValue | RgbaAnimatedValue | ColorValue);
export declare function getRgbaValueAndNativeColor(value: RgbaValue | ColorValue): Readonly<{
  rgbaValue: RgbaValue;
  nativeColor?: NativeColorValue | undefined;
}>;
declare class AnimatedColor extends AnimatedWithChildren {
  r: AnimatedValue;
  g: AnimatedValue;
  b: AnimatedValue;
  a: AnimatedValue;
  nativeColor: null | undefined | NativeColorValue;
  constructor(valueIn?: InputValue, config?: null | undefined | AnimatedColorConfig);
  /**
   * Directly set the value. This will stop any animations running on the value
   * and update all the bound properties.
   */
  setValue(value: RgbaValue | ColorValue): void;
  /**
   * Sets an offset that is applied on top of whatever value is set, whether
   * via `setValue`, an animation, or `Animated.event`. Useful for compensating
   * things like the start of a pan gesture.
   */
  setOffset(offset: RgbaValue): void;
  /**
   * Merges the offset value into the base value and resets the offset to zero.
   * The final output of the value is unchanged.
   */
  flattenOffset(): void;
  /**
   * Sets the offset value to the base value, and resets the base value to
   * zero. The final output of the value is unchanged.
   */
  extractOffset(): void;
  /**
   * Stops any running animation or tracking. `callback` is invoked with the
   * final value after stopping the animation, which is useful for updating
   * state to match the animation position with layout.
   */
  stopAnimation(callback?: ColorListenerCallback): void;
  /**
   * Stops any animation and resets the value to its original.
   */
  resetAnimation(callback?: ColorListenerCallback): void;
}
export default AnimatedColor;
