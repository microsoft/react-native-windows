/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<7a7637275adadb1908758b5e7330cffc>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\nodes\AnimatedProps.js
 */

import * as React from "react";
import type { RootTag } from "../../Types/RootTagTypes";
import type { AnimatedNodeConfig } from "./AnimatedNode";
import type { AnimatedStyleAllowlist } from "./AnimatedStyle";
import AnimatedNode from "./AnimatedNode";
export type AnimatedPropsAllowlist = Readonly<{
  style?: AnimatedStyleAllowlist | undefined;
  [key: string]: true | AnimatedStyleAllowlist;
}>;
type TargetViewInstance = React.ComponentRef<React.ElementType>;
declare class AnimatedProps extends AnimatedNode {
  constructor(inputProps: {
    [$$Key$$: string]: unknown;
  }, callback: () => void, allowlist?: null | undefined | AnimatedPropsAllowlist, rootTag?: RootTag, config?: null | undefined | AnimatedNodeConfig);
  update(): void;
  setNativeView(instance: TargetViewInstance): void;
}
export default AnimatedProps;
