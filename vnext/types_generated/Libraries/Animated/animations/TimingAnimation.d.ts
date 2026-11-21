/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<427a017a79a81979a609e68de344bf33>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\animations\TimingAnimation.js
 */

import type { RgbaValue } from "../nodes/AnimatedColor";
import type AnimatedInterpolation from "../nodes/AnimatedInterpolation";
import type AnimatedValue from "../nodes/AnimatedValue";
import type AnimatedValueXY from "../nodes/AnimatedValueXY";
import type { AnimationConfig, EndCallback } from "./Animation";
import AnimatedColor from "../nodes/AnimatedColor";
import Animation from "./Animation";
export type TimingAnimationConfig = Readonly<Omit<AnimationConfig, "toValue" | "easing" | "duration" | "delay"> & {
  toValue: number | AnimatedValue | Readonly<{
    x: number;
    y: number;
  }> | AnimatedValueXY | RgbaValue | AnimatedColor | AnimatedInterpolation<number>;
  easing?: ((value: number) => number) | undefined;
  duration?: number | undefined;
  delay?: number | undefined;
}>;
export type TimingAnimationConfigSingle = Readonly<Omit<AnimationConfig, "toValue" | "easing" | "duration" | "delay"> & {
  toValue: number;
  easing?: ((value: number) => number) | undefined;
  duration?: number | undefined;
  delay?: number | undefined;
}>;
declare class TimingAnimation extends Animation {
  constructor(config: TimingAnimationConfigSingle);
  start(fromValue: number, onUpdate: (value: number) => void, onEnd: null | undefined | EndCallback, previousAnimation: null | undefined | Animation, animatedValue: AnimatedValue): void;
  onUpdate(): void;
  stop(): void;
}
export default TimingAnimation;
