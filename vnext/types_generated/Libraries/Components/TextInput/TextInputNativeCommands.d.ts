/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<9f51ddf0cd0fdf4e97b74b653cb07a93>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\TextInput\TextInputNativeCommands.js
 */

import type { Int32 } from "../../Types/CodegenTypes";
import * as React from "react";
export interface TextInputNativeCommands<T> {
  readonly focus: (viewRef: React.ComponentRef<T>) => void;
  readonly blur: (viewRef: React.ComponentRef<T>) => void;
  readonly setTextAndSelection: (viewRef: React.ComponentRef<T>, mostRecentEventCount: Int32, value: string | undefined, start: Int32, end: Int32) => void;
}
declare const supportedCommands: string[];
declare const $$TextInputNativeCommands: typeof supportedCommands;
declare type $$TextInputNativeCommands = typeof $$TextInputNativeCommands;
export default $$TextInputNativeCommands;
