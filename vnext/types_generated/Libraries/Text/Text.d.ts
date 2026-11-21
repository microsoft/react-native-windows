/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<7a601bc2b3a9eaf196fdf0f50bf87dd9>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Text\Text.js
 */

import type { HostInstance } from "../../src/private/types/HostInstance";
import type { TextProps } from "./TextProps";
import * as React from "react";
export type TextInstance = HostInstance;
export type { TextProps } from "./TextProps";
declare const TextImpl: (props: Omit<TextProps, "ref"> & {
  ref?: React.Ref<TextInstance> | undefined;
}) => React.ReactNode;
/**
 * A React component for displaying text. `Text` supports nesting, styling,
 * and touch handling.
 *
 * `Text` uses text layout instead of flexbox — elements inside wrap at end of
 * line rather than being positioned as rectangles.
 *
 * @see https://reactnative.dev/docs/text
 */
declare const $$Text: typeof TextImpl;
declare type $$Text = typeof $$Text;
export default $$Text;
