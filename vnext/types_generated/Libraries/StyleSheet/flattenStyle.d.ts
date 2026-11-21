/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<f4a54697d4d5209dc8f522222caf469d>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\StyleSheet\flattenStyle.js
 */

import type AnimatedNode from "../Animated/nodes/AnimatedNode";
import type { ____DangerouslyImpreciseAnimatedStyleProp_Internal, ____FlattenStyleProp_Internal } from "./StyleSheetTypes";
type NonAnimatedNodeObject<TStyleProp> = TStyleProp extends AnimatedNode ? never : TStyleProp;
declare function flattenStyle<TStyleProp extends ____DangerouslyImpreciseAnimatedStyleProp_Internal>(style: null | undefined | TStyleProp): null | undefined | NonAnimatedNodeObject<____FlattenStyleProp_Internal<TStyleProp>>;
declare const $$flattenStyle: typeof flattenStyle;
declare type $$flattenStyle = typeof $$flattenStyle;
export default $$flattenStyle;
