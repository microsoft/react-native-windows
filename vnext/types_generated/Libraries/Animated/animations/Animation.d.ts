/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<548a508a4b736280bb07b3d19f780bfa>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\animations\Animation.js
 */

import type { PlatformConfig } from "../AnimatedPlatformConfig";
import type AnimatedValue from "../nodes/AnimatedValue";
export type EndResult = {
  finished: boolean;
  value?: number | undefined;
  offset?: number | undefined;
};
export type EndCallback = (result: EndResult) => void;
export type AnimationConfig = Readonly<{
  isInteraction?: boolean | undefined;
  useNativeDriver: boolean;
  platformConfig?: PlatformConfig | undefined;
  onComplete?: EndCallback | undefined;
  iterations?: number | undefined;
  isLooping?: boolean | undefined;
  debugID?: string | undefined;
}>;
declare class Animation {
  constructor(config: AnimationConfig);
  start(fromValue: number, onUpdate: (value: number) => void, onEnd: null | undefined | EndCallback, previousAnimation: null | undefined | Animation, animatedValue: AnimatedValue): void;
  stop(): void;
}
export default Animation;
