/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a2577fc251bff31a4a937d30107cb2a8>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\nodes\AnimatedValueXY.js
 */

import type { AnimatedNodeConfig } from "./AnimatedNode";
import AnimatedValue from "./AnimatedValue";
import AnimatedWithChildren from "./AnimatedWithChildren";
export type AnimatedValueXYConfig = Readonly<Omit<AnimatedNodeConfig, "useNativeDriver"> & {
  useNativeDriver: boolean;
}>;
type ValueXYListenerCallback = (value: {
  x: number;
  y: number;
}) => unknown;
/**
 * 2D Value for driving 2D animations, such as pan gestures. Almost identical
 * API to normal `Animated.Value`, but multiplexed.
 *
 * See https://reactnative.dev/docs/animatedvaluexy
 */
declare class AnimatedValueXY extends AnimatedWithChildren {
  x: AnimatedValue;
  y: AnimatedValue;
  constructor(valueIn?: null | undefined | {
    readonly x: number | AnimatedValue;
    readonly y: number | AnimatedValue;
  }, config?: null | undefined | AnimatedValueXYConfig);
  /**
   * Directly set the value. This will stop any animations running on the value
   * and update all the bound properties.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#setvalue
   */
  setValue(value: {
    x: number;
    y: number;
  }): void;
  /**
   * Sets an offset that is applied on top of whatever value is set, whether
   * via `setValue`, an animation, or `Animated.event`. Useful for compensating
   * things like the start of a pan gesture.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#setoffset
   */
  setOffset(offset: {
    x: number;
    y: number;
  }): void;
  /**
   * Merges the offset value into the base value and resets the offset to zero.
   * The final output of the value is unchanged.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#flattenoffset
   */
  flattenOffset(): void;
  /**
   * Sets the offset value to the base value, and resets the base value to
   * zero. The final output of the value is unchanged.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#extractoffset
   */
  extractOffset(): void;
  /**
   * Stops any animation and resets the value to its original.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#resetanimation
   */
  resetAnimation(callback?: (value: {
    x: number;
    y: number;
  }) => void): void;
  /**
   * Stops any running animation or tracking. `callback` is invoked with the
   * final value after stopping the animation, which is useful for updating
   * state to match the animation position with layout.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#stopanimation
   */
  stopAnimation(callback?: (value: {
    x: number;
    y: number;
  }) => void): void;
  /**
   * Adds an asynchronous listener to the value so you can observe updates from
   * animations.  This is useful because there is no way to synchronously read
   * the value because it might be driven natively.
   *
   * Returns a string that serves as an identifier for the listener.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#addlistener
   */
  addListener(callback: ValueXYListenerCallback): string;
  /**
   * Unregister a listener. The `id` param shall match the identifier
   * previously returned by `addListener()`.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#removelistener
   */
  removeListener(id: string): void;
  /**
   * Remove all registered listeners.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#removealllisteners
   */
  removeAllListeners(): void;
  /**
   * Converts `{x, y}` into `{left, top}` for use in style.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#getlayout
   */
  getLayout(): {
    [key: string]: AnimatedValue;
  };
  /**
   * Converts `{x, y}` into a useable translation transform.
   *
   * See https://reactnative.dev/docs/animatedvaluexy#gettranslatetransform
   */
  getTranslateTransform(): Array<{
    translateX: AnimatedValue;
  } | {
    translateY: AnimatedValue;
  }>;
}
export default AnimatedValueXY;
