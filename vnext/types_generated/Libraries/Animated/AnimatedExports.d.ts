/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<2b4b2d3f98ae3da676d3b31f4bd7fba6>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\AnimatedExports.js.flow
 */

import AnimatedImplementation from "./AnimatedImplementation";
export type { CompositeAnimation, Numeric } from "./AnimatedImplementation";
export type { DecayAnimationConfig } from "./animations/DecayAnimation";
export type { SpringAnimationConfig } from "./animations/SpringAnimation";
export type { TimingAnimationConfig } from "./animations/TimingAnimation";
export { default as FlatList } from "./components/AnimatedFlatList";
export { default as Image } from "./components/AnimatedImage";
export { default as ScrollView } from "./components/AnimatedScrollView";
export { default as SectionList } from "./components/AnimatedSectionList";
export { default as Text } from "./components/AnimatedText";
export { default as View } from "./components/AnimatedView";
export { default as Color } from "./nodes/AnimatedColor";
export { AnimatedEvent as Event } from "./AnimatedEvent";
export { default as Interpolation } from "./nodes/AnimatedInterpolation";
export { default as Node } from "./nodes/AnimatedNode";
export { default as Value } from "./nodes/AnimatedValue";
export { default as ValueXY } from "./nodes/AnimatedValueXY";
/** @deprecated Use Animated.Interpolation instead */
export type { default as AnimatedInterpolation } from "./nodes/AnimatedInterpolation";
/** @deprecated Use Animated.Color instead */
export type { default as AnimatedColor } from "./nodes/AnimatedColor";
export type { AnimatedValueConfig as AnimatedConfig } from "./nodes/AnimatedValue";
export type { default as AnimatedNode } from "./nodes/AnimatedNode";
export type { default as AnimatedAddition } from "./nodes/AnimatedAddition";
export type { default as AnimatedDiffClamp } from "./nodes/AnimatedDiffClamp";
export type { default as AnimatedDivision } from "./nodes/AnimatedDivision";
export type { InterpolationConfigType as InterpolationConfig } from "./nodes/AnimatedInterpolation";
export type { default as AnimatedModulo } from "./nodes/AnimatedModulo";
export type { default as AnimatedMultiplication } from "./nodes/AnimatedMultiplication";
export type { default as AnimatedSubtraction } from "./nodes/AnimatedSubtraction";
export type { WithAnimatedValue, AnimatedProps } from "./createAnimatedComponent";
export type { AnimatedComponentType as AnimatedComponent } from "./createAnimatedComponent";
/**
 * Creates a new Animated value composed from two Animated values added
 * together.
 *
 * See https://reactnative.dev/docs/animated#add
 */
export declare const add: typeof AnimatedImplementation.add;
export declare type add = typeof add;
/**
 * Imperative API to attach an animated value to an event on a view. Prefer
 * using `Animated.event` with `useNativeDrive: true` if possible.
 *
 * See https://reactnative.dev/docs/animated#attachnativeevent
 */
export declare const attachNativeEvent: typeof AnimatedImplementation.attachNativeEvent;
export declare type attachNativeEvent = typeof attachNativeEvent;
/**
 * Make any React component Animatable. Used to create `Animated.View`, etc.
 *
 * See https://reactnative.dev/docs/animated#createanimatedcomponent
 */
export declare const createAnimatedComponent: typeof AnimatedImplementation.createAnimatedComponent;
export declare type createAnimatedComponent = typeof createAnimatedComponent;
/**
 * Animates a value from an initial velocity to zero based on a decay
 * coefficient.
 *
 * See https://reactnative.dev/docs/animated#decay
 */
export declare const decay: typeof AnimatedImplementation.decay;
export declare type decay = typeof decay;
/**
 * Starts an animation after the given delay.
 *
 * See https://reactnative.dev/docs/animated#delay
 */
export declare const delay: typeof AnimatedImplementation.delay;
export declare type delay = typeof delay;
/**
 * Create a new Animated value that is limited between 2 values. It uses the
 * difference between the last value so even if the value is far from the
 * bounds it will start changing when the value starts getting closer again.
 *
 * See https://reactnative.dev/docs/animated#diffclamp
 */
export declare const diffClamp: typeof AnimatedImplementation.diffClamp;
export declare type diffClamp = typeof diffClamp;
/**
 * Creates a new Animated value composed by dividing the first Animated value
 * by the second Animated value.
 *
 * See https://reactnative.dev/docs/animated#divide
 */
export declare const divide: typeof AnimatedImplementation.divide;
export declare type divide = typeof divide;
/**
 * Takes an array of mappings and extracts values from each arg accordingly,
 * then calls `setValue` on the mapped outputs.
 *
 * See https://reactnative.dev/docs/animated#event
 */
export declare const event: typeof AnimatedImplementation.event;
export declare type event = typeof event;
/**
 * Advanced imperative API for snooping on animated events that are passed in
 * through props. Use values directly where possible.
 *
 * See https://reactnative.dev/docs/animated#forkevent
 */
export declare const forkEvent: typeof AnimatedImplementation.forkEvent;
export declare type forkEvent = typeof forkEvent;
/**
 * Loops a given animation continuously, so that each time it reaches the
 * end, it resets and begins again from the start.
 *
 * See https://reactnative.dev/docs/animated#loop
 */
export declare const loop: typeof AnimatedImplementation.loop;
export declare type loop = typeof loop;
/**
 * Creates a new Animated value that is the (non-negative) modulo of the
 * provided Animated value.
 *
 * See https://reactnative.dev/docs/animated#modulo
 */
export declare const modulo: typeof AnimatedImplementation.modulo;
export declare type modulo = typeof modulo;
/**
 * Creates a new Animated value composed from two Animated values multiplied
 * together.
 *
 * See https://reactnative.dev/docs/animated#multiply
 */
export declare const multiply: typeof AnimatedImplementation.multiply;
export declare type multiply = typeof multiply;
/**
 * Starts an array of animations all at the same time. By default, if one
 * of the animations is stopped, they will all be stopped. You can override
 * this with the `stopTogether` flag.
 *
 * See https://reactnative.dev/docs/animated#parallel
 */
export declare const parallel: typeof AnimatedImplementation.parallel;
export declare type parallel = typeof parallel;
/**
 * Starts an array of animations in order, waiting for each to complete
 * before starting the next. If the current running animation is stopped, no
 * following animations will be started.
 *
 * See https://reactnative.dev/docs/animated#sequence
 */
export declare const sequence: typeof AnimatedImplementation.sequence;
export declare type sequence = typeof sequence;
/**
 * Animates a value according to an analytical spring model based on
 * damped harmonic oscillation.
 *
 * See https://reactnative.dev/docs/animated#spring
 */
export declare const spring: typeof AnimatedImplementation.spring;
export declare type spring = typeof spring;
/**
 * Array of animations may run in parallel (overlap), but are started in
 * sequence with successive delays. Nice for doing trailing effects.
 *
 * See https://reactnative.dev/docs/animated#stagger
 */
export declare const stagger: typeof AnimatedImplementation.stagger;
export declare type stagger = typeof stagger;
/**
 * Creates a new Animated value composed by subtracting the second Animated
 * value from the first Animated value.
 *
 * See https://reactnative.dev/docs/animated#subtract
 */
export declare const subtract: typeof AnimatedImplementation.subtract;
export declare type subtract = typeof subtract;
/**
 * Animates a value along a timed easing curve. The Easing module has tons of
 * predefined curves, or you can use your own function.
 *
 * See https://reactnative.dev/docs/animated#timing
 */
export declare const timing: typeof AnimatedImplementation.timing;
export declare type timing = typeof timing;
export declare const unforkEvent: typeof AnimatedImplementation.unforkEvent;
export declare type unforkEvent = typeof unforkEvent;
