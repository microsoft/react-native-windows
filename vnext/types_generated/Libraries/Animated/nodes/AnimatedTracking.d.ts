/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<d76750d525c1551f53858b921e269b74>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\nodes\AnimatedTracking.js
 */

import type Animation from "../animations/Animation";
import type { AnimationConfig, EndCallback } from "../animations/Animation";
import type { AnimatedNodeConfig } from "./AnimatedNode";
import type AnimatedValue from "./AnimatedValue";
import AnimatedNode from "./AnimatedNode";
type TrackingAnimationConfig = Readonly<Omit<AnimationConfig, "toValue"> & {
  toValue: AnimatedNode;
}>;
declare class AnimatedTracking extends AnimatedNode {
  constructor(value: AnimatedValue, parent: AnimatedNode, animationClass: new (...args: any[]) => Animation, animationConfig: TrackingAnimationConfig, callback?: null | undefined | EndCallback, config?: null | undefined | AnimatedNodeConfig);
  update(): void;
}
export default AnimatedTracking;
