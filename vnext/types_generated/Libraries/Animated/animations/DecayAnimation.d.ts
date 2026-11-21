/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<76b86e970c28ef5e38cc4729ff7619f3>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\animations\DecayAnimation.js
 */

import type AnimatedValue from "../nodes/AnimatedValue";
import type { AnimationConfig, EndCallback } from "./Animation";
import Animation from "./Animation";
export type DecayAnimationConfig = Readonly<Omit<AnimationConfig, "velocity" | "deceleration"> & {
  velocity: number | Readonly<{
    x: number;
    y: number;
  }>;
  deceleration?: number | undefined;
}>;
export type DecayAnimationConfigSingle = Readonly<Omit<AnimationConfig, "velocity" | "deceleration"> & {
  velocity: number;
  deceleration?: number | undefined;
}>;
declare class DecayAnimation extends Animation {
  constructor(config: DecayAnimationConfigSingle);
  start(fromValue: number, onUpdate: (value: number) => void, onEnd: null | undefined | EndCallback, previousAnimation: null | undefined | Animation, animatedValue: AnimatedValue): void;
  onUpdate(): void;
  stop(): void;
}
export default DecayAnimation;
