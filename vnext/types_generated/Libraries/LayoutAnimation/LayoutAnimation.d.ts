/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<7fe1456d3330c8318057f54a4cbccfe8>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\LayoutAnimation\LayoutAnimation.js
 */

import type { LayoutAnimationConfig, LayoutAnimationProperty, LayoutAnimationType } from "../Renderer/shims/ReactNativeTypes";
export type { LayoutAnimationType, LayoutAnimationProperty, LayoutAnimationAnimationConfig as LayoutAnimationAnim } from "../Renderer/shims/ReactNativeTypes";
export type { LayoutAnimationConfig } from "../Renderer/shims/ReactNativeTypes";
export type LayoutAnimationTypes = Readonly<{ [type in LayoutAnimationType]: type }>;
export type LayoutAnimationProperties = Readonly<{ [prop in LayoutAnimationProperty]: prop }>;
type OnAnimationDidEndCallback = () => void;
type OnAnimationDidFailCallback = () => void;
declare function setLayoutAnimationEnabled(value: boolean): void;
/**
 * Configures the next commit to be animated.
 *
 * onAnimationDidEnd is guaranteed to be called when the animation completes.
 * onAnimationDidFail is *never* called in the classic, pre-Fabric renderer,
 * and never has been. In the new renderer (Fabric) it is called only if configuration
 * parsing fails.
 */
declare function configureNext(config: LayoutAnimationConfig, onAnimationDidEnd?: OnAnimationDidEndCallback, onAnimationDidFail?: OnAnimationDidFailCallback): void;
declare function createLayoutAnimation(duration: number, type?: LayoutAnimationType, property?: LayoutAnimationProperty): LayoutAnimationConfig;
declare const Presets: {
  easeInEaseOut: LayoutAnimationConfig;
  linear: LayoutAnimationConfig;
  spring: LayoutAnimationConfig;
};
declare const LayoutAnimation: {
  /**
   * Schedule an animation for the next layout. Config includes duration,
   * create/update/delete sub-configs.
   *
   * @param config Specifies animation properties:
   *
   *   - `duration` in milliseconds
   *   - `create`, `AnimationConfig` for animating in new views
   *   - `update`, `AnimationConfig` for animating views that have been updated
   *
   * @param onAnimationDidEnd Called when the animation finished.
   * Only supported on iOS.
   * @param onError Called on error. Only supported on iOS.
   */
  configureNext: typeof configureNext;
  /**
   * Helper that creates a config object for `configureNext`.
   */
  create: typeof createLayoutAnimation;
  /**
   * Enum of animation types (spring, linear, easeInEaseOut, easeIn, easeOut,
   * keyboard).
   */
  Types: LayoutAnimationTypes;
  /**
   * Enum of layout properties to animate (opacity, scaleX, scaleY, scaleXY).
   */
  Properties: LayoutAnimationProperties;
  checkConfig(...args: Array<unknown>): void;
  /**
   * Predefined configs (easeInEaseOut, linear, spring).
   */
  Presets: typeof Presets;
  easeInEaseOut: (onAnimationDidEnd?: OnAnimationDidEndCallback) => void;
  linear: (onAnimationDidEnd?: OnAnimationDidEndCallback) => void;
  spring: (onAnimationDidEnd?: OnAnimationDidEndCallback) => void;
  setEnabled: typeof setLayoutAnimationEnabled;
};
/**
 * Automatically animates views to their new positions when the next layout
 * happens. Call before updating state to animate the transition.
 *
 * Note that in order to get this to work on **Android** you need to set the following flags via `UIManager`:
 *
 *     UIManager.setLayoutAnimationEnabledExperimental && UIManager.setLayoutAnimationEnabledExperimental(true);
 *
 * @see https://reactnative.dev/docs/layoutanimation
 */
declare const $$LayoutAnimation: typeof LayoutAnimation;
declare type $$LayoutAnimation = typeof $$LayoutAnimation;
export default $$LayoutAnimation;
