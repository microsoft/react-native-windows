/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<16100ba7dfda43816a2a4f2b59dc3626>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\AnimatedImplementation.js
 */

import type { EventConfig, Mapping } from "./AnimatedEvent";
import type { EndCallback } from "./animations/Animation";
import type { DecayAnimationConfig } from "./animations/DecayAnimation";
import type { SpringAnimationConfig } from "./animations/SpringAnimation";
import type { TimingAnimationConfig } from "./animations/TimingAnimation";
import { AnimatedEvent, attachNativeEventImpl } from "./AnimatedEvent";
import createAnimatedComponent from "./createAnimatedComponent";
import AnimatedAddition from "./nodes/AnimatedAddition";
import AnimatedColor from "./nodes/AnimatedColor";
import AnimatedDiffClamp from "./nodes/AnimatedDiffClamp";
import AnimatedDivision from "./nodes/AnimatedDivision";
import AnimatedInterpolation from "./nodes/AnimatedInterpolation";
import AnimatedModulo from "./nodes/AnimatedModulo";
import AnimatedMultiplication from "./nodes/AnimatedMultiplication";
import AnimatedNode from "./nodes/AnimatedNode";
import AnimatedSubtraction from "./nodes/AnimatedSubtraction";
import AnimatedValue from "./nodes/AnimatedValue";
import AnimatedValueXY from "./nodes/AnimatedValueXY";
export type CompositeAnimation = {
  /**
   * Animations are started by calling start() on your animation.
   * start() takes a completion callback that will be called when the
   * animation is done or when the animation is done because stop() was
   * called on it before it could finish.
   *
   * @param callback - Optional function that will be called
   *      after the animation finished running normally or when the animation
   *      is done because stop() was called on it before it could finish
   *
   * @example
   *   Animated.timing({}).start(({ finished }) => {
   *    // completion callback
   *   });
   */
  start: (callback?: EndCallback | undefined, isLooping?: boolean) => void;
  /**
   * Stops any running animation.
   */
  stop: () => void;
  /**
   * Stops any running animation and resets the value to its original.
   */
  reset: () => void;
};
declare const addImpl: (a: AnimatedNode | number, b: AnimatedNode | number) => AnimatedAddition;
declare const subtractImpl: (a: AnimatedNode | number, b: AnimatedNode | number) => AnimatedSubtraction;
declare const divideImpl: (a: AnimatedNode | number, b: AnimatedNode | number) => AnimatedDivision;
declare const multiplyImpl: (a: AnimatedNode | number, b: AnimatedNode | number) => AnimatedMultiplication;
declare const moduloImpl: (a: AnimatedNode, modulus: number) => AnimatedModulo;
declare const diffClampImpl: (a: AnimatedNode, min: number, max: number) => AnimatedDiffClamp;
declare const springImpl: (value: AnimatedValue | AnimatedValueXY | AnimatedColor, config: SpringAnimationConfig) => CompositeAnimation;
declare const timingImpl: (value: AnimatedValue | AnimatedValueXY | AnimatedColor, config: TimingAnimationConfig) => CompositeAnimation;
declare const decayImpl: (value: AnimatedValue | AnimatedValueXY | AnimatedColor, config: DecayAnimationConfig) => CompositeAnimation;
declare const sequenceImpl: (animations: Array<CompositeAnimation>) => CompositeAnimation;
type ParallelConfig = {
  stopTogether?: boolean | undefined;
};
declare const parallelImpl: (animations: Array<CompositeAnimation>, config?: null | undefined | ParallelConfig) => CompositeAnimation;
declare const delayImpl: (time: number) => CompositeAnimation;
declare const staggerImpl: (time: number, animations: Array<CompositeAnimation>) => CompositeAnimation;
type LoopAnimationConfig = {
  iterations: number;
  resetBeforeIteration?: boolean | undefined;
};
declare const loopImpl: (animation: CompositeAnimation, $$PARAM_1$$?: LoopAnimationConfig) => CompositeAnimation;
declare function forkEventImpl(event: (null | undefined | AnimatedEvent) | (null | undefined | Function), listener: Function): AnimatedEvent | Function;
declare function unforkEventImpl(event: (null | undefined | AnimatedEvent) | (null | undefined | Function), listener: Function): void;
declare const eventImpl: <T>(argMapping: ReadonlyArray<null | undefined | Mapping>, config: EventConfig<T>) => (...args: Array<any>) => void;
type AnimatedNumeric = AnimatedAddition | AnimatedDiffClamp | AnimatedDivision | AnimatedInterpolation<number> | AnimatedModulo | AnimatedMultiplication | AnimatedSubtraction | AnimatedValue;
export type { AnimatedNumeric as Numeric };
/**
 * The `Animated` library is designed to make animations fluid, powerful, and
 * easy to build and maintain. `Animated` focuses on declarative relationships
 * between inputs and outputs, with configurable transforms in between, and
 * simple `start`/`stop` methods to control time-based animation execution.
 * If additional transforms are added, be sure to include them in
 * AnimatedMock.js as well.
 *
 * See https://reactnative.dev/docs/animated
 */
declare const $$AnimatedImplementation: {
  /**
   * Standard value class for driving animations.  Typically initialized with
   * `new Animated.Value(0);`
   *
   * See https://reactnative.dev/docs/animated#value
   */
  Value: typeof AnimatedValue;
  /**
   * 2D value class for driving 2D animations, such as pan gestures.
   *
   * See https://reactnative.dev/docs/animatedvaluexy
   */
  ValueXY: typeof AnimatedValueXY;
  /**
   * Value class for driving color animations.
   */
  Color: typeof AnimatedColor;
  /**
   * Exported to use the Interpolation type in flow.
   *
   * See https://reactnative.dev/docs/animated#interpolation
   */
  Interpolation: typeof AnimatedInterpolation;
  /**
   * Exported for ease of type checking. All animated values derive from this
   * class.
   *
   * See https://reactnative.dev/docs/animated#node
   */
  Node: typeof AnimatedNode;
  /**
   * Animates a value from an initial velocity to zero based on a decay
   * coefficient.
   */
  decay: typeof decayImpl;
  /**
   * Animates a value along a timed easing curve.  The `Easing` module has tons
   * of pre-defined curves, or you can use your own function.
   */
  timing: typeof timingImpl;
  /**
   * Spring animation based on Rebound and Origami.  Tracks velocity state to
   * create fluid motions as the `toValue` updates, and can be chained together.
   */
  spring: typeof springImpl;
  /**
   * Creates a new Animated value composed from two Animated values added
   * together.
   */
  add: typeof addImpl;
  /**
   * Creates a new Animated value composed by subtracting the second Animated
   * value from the first Animated value.
   */
  subtract: typeof subtractImpl;
  /**
   * Creates a new Animated value composed by dividing the first Animated
   * value by the second Animated value.
   */
  divide: typeof divideImpl;
  /**
   * Creates a new Animated value composed from two Animated values multiplied
   * together.
   */
  multiply: typeof multiplyImpl;
  /**
   * Creates a new Animated value that is the (non-negative) modulo of the
   * provided Animated value
   */
  modulo: typeof moduloImpl;
  /**
   * Create a new Animated value that is limited between 2 values. It uses the
   * difference between the last value so even if the value is far from the bounds
   * it will start changing when the value starts getting closer again.
   * (`value = clamp(value + diff, min, max)`).
   *
   * This is useful with scroll events, for example, to show the navbar when
   * scrolling up and to hide it when scrolling down.
   */
  diffClamp: typeof diffClampImpl;
  /**
   * Starts an animation after the given delay.
   */
  delay: typeof delayImpl;
  /**
   * Starts an array of animations in order, waiting for each to complete
   * before starting the next.  If the current running animation is stopped, no
   * following animations will be started.
   */
  sequence: typeof sequenceImpl;
  /**
   * Starts an array of animations all at the same time.  By default, if one
   * of the animations is stopped, they will all be stopped.  You can override
   * this with the `stopTogether` flag.
   */
  parallel: typeof parallelImpl;
  /**
   * Array of animations may run in parallel (overlap), but are started in
   * sequence with successive delays.  Nice for doing trailing effects.
   */
  stagger: typeof staggerImpl;
  /**
   * Loops a given animation continuously, so that each time it reaches the end,
   * it resets and begins again from the start. Can specify number of times to
   * loop using the key 'iterations' in the config. Will loop without blocking
   * the UI thread if the child animation is set to 'useNativeDriver'.
   */
  loop: typeof loopImpl;
  /**
   *  Takes an array of mappings and extracts values from each arg accordingly,
   *  then calls `setValue` on the mapped outputs.  e.g.
   *
   *```javascript
   *  onScroll={Animated.event(
   *    [{nativeEvent: {contentOffset: {x: this._scrollX}}}]
   *    {listener},          // Optional async listener
   *  )
   *  ...
   *  onPanResponderMove: Animated.event([
   *    null,                // raw event arg ignored
   *    {dx: this._panX},    // gestureState arg
   *  ]),
   *```
   */
  event: typeof eventImpl;
  /**
   * Make any React component Animatable.  Used to create `Animated.View`, etc.
   */
  createAnimatedComponent: typeof createAnimatedComponent;
  attachNativeEvent: typeof attachNativeEventImpl;
  forkEvent: typeof forkEventImpl;
  unforkEvent: typeof unforkEventImpl;
  /**
   * Expose Event class, so it can be used as a type for type checkers.
   */
  Event: typeof AnimatedEvent;
};
declare type $$AnimatedImplementation = typeof $$AnimatedImplementation;
export default $$AnimatedImplementation;
