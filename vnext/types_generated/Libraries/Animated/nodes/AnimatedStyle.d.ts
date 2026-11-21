/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<293caccabf6b2124b4c22de2ba4b9562>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Animated\nodes\AnimatedStyle.js
 */

import type { AnimatedNodeConfig } from "./AnimatedNode";
import AnimatedNode from "./AnimatedNode";
import AnimatedWithChildren from "./AnimatedWithChildren";
export type AnimatedStyleAllowlist = Readonly<{
  [$$Key$$: string]: true;
}>;
type FlatStyle = {
  [$$Key$$: string]: unknown;
};
declare class AnimatedStyle extends AnimatedWithChildren {
  /**
   * Creates an `AnimatedStyle` if `value` contains `AnimatedNode` instances.
   * Otherwise, returns `null`.
   */
  static from(flatStyle: null | undefined | FlatStyle, allowlist: null | undefined | AnimatedStyleAllowlist, originalStyleForWeb: null | undefined | unknown): null | undefined | AnimatedStyle;
  constructor(nodeKeys: ReadonlyArray<string>, nodes: ReadonlyArray<AnimatedNode>, style: {
    [$$Key$$: string]: unknown;
  }, originalStyleForWeb: null | undefined | unknown, config?: null | undefined | AnimatedNodeConfig);
}
export default AnimatedStyle;
