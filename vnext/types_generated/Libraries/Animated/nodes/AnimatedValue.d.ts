/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a1c1be0918c6540456d19f9ac726ad24>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\nodes\AnimatedValue.js
 */

import type Animation from "../animations/Animation";
import type { EndCallback } from "../animations/Animation";
import type { InterpolationConfigSupportedOutputType, InterpolationConfigType } from "./AnimatedInterpolation";
import type AnimatedNode from "./AnimatedNode";
import type { AnimatedNodeConfig, ValueListenerCallback } from "./AnimatedNode";
import type AnimatedTracking from "./AnimatedTracking";
import AnimatedInterpolation from "./AnimatedInterpolation";
import AnimatedWithChildren from "./AnimatedWithChildren";
export type AnimatedValueConfig = Readonly<Omit<AnimatedNodeConfig, "useNativeDriver"> & {
  useNativeDriver: boolean;
}>;
/**
 * Animated works by building a directed acyclic graph of dependencies
 * transparently when you render your Animated components.
 *
 *               new Animated.Value(0)
 *     .interpolate()        .interpolate()    new Animated.Value(1)
 *         opacity               translateY      scale
 *          style                         transform
 *         View#234                         style
 *                                         View#123
 *
 * A) Top Down phase
 * When an Animated.Value is updated, we recursively go down through this
 * graph in order to find leaf nodes: the views that we flag as needing
 * an update.
 *
 * B) Bottom Up phase
 * When a view is flagged as needing an update, we recursively go back up
 * in order to build the new value that it needs. The reason why we need
 * this two-phases process is to deal with composite props such as
 * transform which can receive values from multiple parents.
 */
export declare function flushValue(rootNode: AnimatedNode): void;
/**
 * Standard value for driving animations.  One `Animated.Value` can drive
 * multiple properties in a synchronized fashion, but can only be driven by one
 * mechanism at a time.  Using a new mechanism (e.g. starting a new animation,
 * or calling `setValue`) will stop any previous ones.
 *
 * See https://reactnative.dev/docs/animatedvalue
 */
declare class AnimatedValue extends AnimatedWithChildren {
  constructor(value: number, config?: null | undefined | AnimatedValueConfig);
  /**
   * Narrows `AnimatedNode.addListener`: the value of an `Animated.Value` is
   * always a number, so listeners always receive `{value: number}`.
   */
  addListener(callback: ValueListenerCallback): string;
  /**
   * Directly set the value.  This will stop any animations running on the value
   * and update all the bound properties.
   *
   * See https://reactnative.dev/docs/animatedvalue#setvalue
   */
  setValue(value: number): void;
  /**
   * Sets an offset that is applied on top of whatever value is set, whether via
   * `setValue`, an animation, or `Animated.event`.  Useful for compensating
   * things like the start of a pan gesture.
   *
   * See https://reactnative.dev/docs/animatedvalue#setoffset
   */
  setOffset(offset: number): void;
  /**
   * Merges the offset value into the base value and resets the offset to zero.
   * The final output of the value is unchanged.
   *
   * See https://reactnative.dev/docs/animatedvalue#flattenoffset
   */
  flattenOffset(): void;
  /**
   * Sets the offset value to the base value, and resets the base value to zero.
   * The final output of the value is unchanged.
   *
   * See https://reactnative.dev/docs/animatedvalue#extractoffset
   */
  extractOffset(): void;
  /**
   * Stops any running animation or tracking. `callback` is invoked with the
   * final value after stopping the animation, which is useful for updating
   * state to match the animation position with layout.
   *
   * See https://reactnative.dev/docs/animatedvalue#stopanimation
   */
  stopAnimation(callback?: null | undefined | ((value: number) => void)): void;
  /**
   * Stops any animation and resets the value to its original.
   *
   * See https://reactnative.dev/docs/animatedvalue#resetanimation
   */
  resetAnimation(callback?: null | undefined | ((value: number) => void)): void;
  /**
   * Interpolates the value before updating the property, e.g. mapping 0-1 to
   * 0-10.
   */
  interpolate<OutputT extends InterpolationConfigSupportedOutputType>(config: InterpolationConfigType<OutputT>): AnimatedInterpolation<OutputT>;
  /**
   * Typically only used internally, but could be used by a custom Animation
   * class.
   *
   * See https://reactnative.dev/docs/animatedvalue#animate
   */
  animate(animation: Animation, callback: null | undefined | EndCallback): void;
  /**
   * Typically only used internally.
   */
  stopTracking(): void;
  /**
   * Typically only used internally.
   */
  track(tracking: AnimatedTracking): void;
}
export default AnimatedValue;
