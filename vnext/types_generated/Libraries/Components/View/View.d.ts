/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<1da23fa76738123e6bb7f3d1777cf911>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\View\View.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { ViewProps } from "./ViewPropTypes";
import * as React from "react";
export type ViewInstance = HostInstance;
declare function View(props: Omit<ViewProps, "ref"> & {
  ref?: React.Ref<ViewInstance> | undefined;
}): React.ReactNode;
/**
 * The most fundamental component for building a UI. `View` is a container that
 * supports layout with flexbox, style, some touch handling, and accessibility
 * controls. `View` maps directly to the native view equivalent on whatever
 * platform React Native is running on, whether that is a `UIView`, `<div>`,
 * `android.view`, etc. `View` is designed to be nested inside other views and
 * can have 0 to many children of any type.
 *
 * @see https://reactnative.dev/docs/view
 */
declare const $$View: typeof View;
declare type $$View = typeof $$View;
export default $$View;
