/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<aae6dfd21cac10d607f72c67d9db0d74>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\Easing.js
 */

export type EasingFunction = (t: number) => number;
declare const EasingStatic: {
  /**
   * A stepping function, returns 1 for any positive value of `n`.
   */
  step0(n: number): number;
  /**
   * A stepping function, returns 1 if `n` is greater than or equal to 1.
   */
  step1(n: number): number;
  /**
   * A linear function, `f(t) = t`. Position correlates to elapsed time one to
   * one.
   *
   * http://cubic-bezier.com/#0,0,1,1
   */
  linear(t: number): number;
  /**
   * A simple inertial interaction, similar to an object slowly accelerating to
   * speed.
   *
   * http://cubic-bezier.com/#.42,0,1,1
   */
  ease(t: number): number;
  /**
   * A quadratic function, `f(t) = t * t`. Position equals the square of elapsed
   * time.
   *
   * http://easings.net/#easeInQuad
   */
  quad(t: number): number;
  /**
   * A cubic function, `f(t) = t * t * t`. Position equals the cube of elapsed
   * time.
   *
   * http://easings.net/#easeInCubic
   */
  cubic(t: number): number;
  /**
   * A power function. Position is equal to the Nth power of elapsed time.
   *
   * n = 4: http://easings.net/#easeInQuart
   * n = 5: http://easings.net/#easeInQuint
   */
  poly(n: number): EasingFunction;
  /**
   * A sinusoidal function.
   *
   * http://easings.net/#easeInSine
   */
  sin(t: number): number;
  /**
   * A circular function.
   *
   * http://easings.net/#easeInCirc
   */
  circle(t: number): number;
  /**
   * An exponential function.
   *
   * http://easings.net/#easeInExpo
   */
  exp(t: number): number;
  /**
   * A simple elastic interaction, similar to a spring oscillating back and
   * forth.
   *
   * Default bounciness is 1, which overshoots a little bit once. 0 bounciness
   * doesn't overshoot at all, and bounciness of N > 1 will overshoot about N
   * times.
   *
   * http://easings.net/#easeInElastic
   */
  elastic(bounciness?: number): EasingFunction;
  /**
   * Use with `Animated.parallel()` to create a simple effect where the object
   * animates back slightly as the animation starts.
   *
   * https://easings.net/#easeInBack
   */
  back(s?: number): EasingFunction;
  /**
   * Provides a simple bouncing effect.
   *
   * http://easings.net/#easeInBounce
   */
  bounce(t: number): number;
  /**
   * Provides a cubic bezier curve, equivalent to CSS Transitions'
   * `transition-timing-function`.
   *
   * A useful tool to visualize cubic bezier curves can be found at
   * http://cubic-bezier.com/
   */
  bezier(x1: number, y1: number, x2: number, y2: number): EasingFunction;
  /**
   * Runs an easing function forwards.
   */
  in(easing: EasingFunction): EasingFunction;
  /**
   * Runs an easing function backwards.
   */
  out(easing: EasingFunction): EasingFunction;
  /**
   * Makes any easing function symmetrical. The easing function will run
   * forwards for half of the duration, then backwards for the rest of the
   * duration.
   */
  inOut(easing: EasingFunction): EasingFunction;
};
export type Easing = typeof EasingStatic;
/**
 * Implements common easing functions for use with `Animated.timing()` to convey
 * physically believable motion in animations.
 *
 * You can find a visualization of some common easing functions at
 * http://easings.net/
 *
 * ### Predefined animations
 *
 * The `Easing` module provides several predefined animations through the
 * following methods:
 *
 * - [`back`](docs/easing.html#back) provides a simple animation where the
 *   object goes slightly back before moving forward
 * - [`bounce`](docs/easing.html#bounce) provides a bouncing animation
 * - [`ease`](docs/easing.html#ease) provides a simple inertial animation
 * - [`elastic`](docs/easing.html#elastic) provides a simple spring interaction
 *
 * ### Standard functions
 *
 * Three standard easing functions are provided:
 *
 * - [`linear`](docs/easing.html#linear)
 * - [`quad`](docs/easing.html#quad)
 * - [`cubic`](docs/easing.html#cubic)
 *
 * The [`poly`](docs/easing.html#poly) function can be used to implement
 * quartic, quintic, and other higher power functions.
 *
 * ### Additional functions
 *
 * Additional mathematical functions are provided by the following methods:
 *
 * - [`bezier`](docs/easing.html#bezier) provides a cubic bezier curve
 * - [`circle`](docs/easing.html#circle) provides a circular function
 * - [`sin`](docs/easing.html#sin) provides a sinusoidal function
 * - [`exp`](docs/easing.html#exp) provides an exponential function
 *
 * The following helpers are used to modify other easing functions.
 *
 * - [`in`](docs/easing.html#in) runs an easing function forwards
 * - [`inOut`](docs/easing.html#inout) makes any easing function symmetrical
 * - [`out`](docs/easing.html#out) runs an easing function backwards
 *
 * @see https://reactnative.dev/docs/easing
 */
declare const $$Easing: typeof EasingStatic;
declare type $$Easing = typeof $$Easing;
export default $$Easing;
