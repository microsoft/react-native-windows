/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<1e014697b3611543a7c8d72742adf507>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\nodes\AnimatedDivision.js
 */

import type { InterpolationConfigType } from "./AnimatedInterpolation";
import type { AnimatedNodeConfig } from "./AnimatedNode";
import AnimatedInterpolation from "./AnimatedInterpolation";
import AnimatedNode from "./AnimatedNode";
import AnimatedWithChildren from "./AnimatedWithChildren";
declare class AnimatedDivision extends AnimatedWithChildren {
  constructor(a: AnimatedNode | number, b: AnimatedNode | number, config?: null | undefined | AnimatedNodeConfig);
  interpolate<OutputT extends number | string>(config: InterpolationConfigType<OutputT>): AnimatedInterpolation<OutputT>;
}
export default AnimatedDivision;
