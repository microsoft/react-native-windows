/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<81b829f302b5f1eb9f7b6e64b5d80ccd>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\animations\SpringAnimation.js
 */

import type AnimatedInterpolation from "../nodes/AnimatedInterpolation";
import type AnimatedValue from "../nodes/AnimatedValue";
import type AnimatedValueXY from "../nodes/AnimatedValueXY";
import type { AnimationConfig, EndCallback } from "./Animation";
import AnimatedColor from "../nodes/AnimatedColor";
import Animation from "./Animation";
export type SpringAnimationConfig = Readonly<Omit<AnimationConfig, "toValue" | "overshootClamping" | "restDisplacementThreshold" | "restSpeedThreshold" | "velocity" | "bounciness" | "speed" | "tension" | "friction" | "stiffness" | "damping" | "mass" | "delay"> & {
  toValue: number | AnimatedValue | {
    x: number;
    y: number;
  } | AnimatedValueXY | {
    r: number;
    g: number;
    b: number;
    a: number;
  } | AnimatedColor | AnimatedInterpolation<number>;
  overshootClamping?: boolean | undefined;
  restDisplacementThreshold?: number | undefined;
  restSpeedThreshold?: number | undefined;
  velocity?: number | Readonly<{
    x: number;
    y: number;
  }> | undefined;
  bounciness?: number | undefined;
  speed?: number | undefined;
  tension?: number | undefined;
  friction?: number | undefined;
  stiffness?: number | undefined;
  damping?: number | undefined;
  mass?: number | undefined;
  delay?: number | undefined;
}>;
export type SpringAnimationConfigSingle = Readonly<Omit<AnimationConfig, "toValue" | "overshootClamping" | "restDisplacementThreshold" | "restSpeedThreshold" | "velocity" | "bounciness" | "speed" | "tension" | "friction" | "stiffness" | "damping" | "mass" | "delay"> & {
  toValue: number;
  overshootClamping?: boolean | undefined;
  restDisplacementThreshold?: number | undefined;
  restSpeedThreshold?: number | undefined;
  velocity?: number | undefined;
  bounciness?: number | undefined;
  speed?: number | undefined;
  tension?: number | undefined;
  friction?: number | undefined;
  stiffness?: number | undefined;
  damping?: number | undefined;
  mass?: number | undefined;
  delay?: number | undefined;
}>;
declare type SpringAnimationInternalState = symbol & {
  __SpringAnimationInternalState__: string;
};
declare class SpringAnimation extends Animation {
  constructor(config: SpringAnimationConfigSingle);
  start(fromValue: number, onUpdate: (value: number) => void, onEnd: null | undefined | EndCallback, previousAnimation: null | undefined | Animation, animatedValue: AnimatedValue): void;
  getInternalState(): SpringAnimationInternalState;
  /**
   * This spring model is based off of a damped harmonic oscillator
   * (https://en.wikipedia.org/wiki/Harmonic_oscillator#Damped_harmonic_oscillator).
   *
   * We use the closed form of the second order differential equation:
   *
   * x'' + (2ζ⍵_0)x' + ⍵^2x = 0
   *
   * where
   *    ⍵_0 = √(k / m) (undamped angular frequency of the oscillator),
   *    ζ = c / 2√mk (damping ratio),
   *    c = damping constant
   *    k = stiffness
   *    m = mass
   *
   * The derivation of the closed form is described in detail here:
   * http://planetmath.org/sites/default/files/texpdf/39745.pdf
   *
   * This algorithm happens to match the algorithm used by CASpringAnimation,
   * a QuartzCore (iOS) API that creates spring animations.
   */
  onUpdate(): void;
  stop(): void;
}
export default SpringAnimation;
