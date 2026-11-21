/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a9ffcae9167dca869706524bfd1e2748>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\components\virtualview\VirtualViewNativeComponent.js
 */

import type { ViewProps } from "../../../../Libraries/Components/View/ViewPropTypes";
import type { DirectEventHandler, Double, Int32, WithDefault } from "../../../../Libraries/Types/CodegenTypes";
import type { HostComponent } from "../../types/HostComponent";
export type NativeModeChangeEvent = Readonly<{
  /**
   * Virtualization mode of the target view.
   *
   * - `0`: Target view is visible.
   * - `1`: Target view is hidden, but can be prerendered.
   * - `2`: Target view is hidden.
   *
   * WORKAROUND: As of this writing, codegen doesn't support enums, so we need
   * to convert `number` into an enum in `VirtualView`.
   */
  mode: Int32;
  /**
   * Rect of the target view, relative to the nearest ancestor scroll container.
   */
  targetRect: Readonly<{
    x: Double;
    y: Double;
    width: Double;
    height: Double;
  }>;
  /**
   * Rect of the threshold that determines the mode of the target view, relative
   * to the nearest ancestor scroll container.
   *
   * - `Visible`: Rect in which the target view is visible.
   * - `Prerender`: Rect in which the target view is prerendered.
   * - `Hidden`: Unused, without any guarantees.
   *
   * This can be used to determine whether and how much new content to render.
   */
  thresholdRect: Readonly<{
    x: Double;
    y: Double;
    width: Double;
    height: Double;
  }>;
}>;
type VirtualViewNativeProps = Readonly<Omit<ViewProps, "initialHidden" | "removeClippedSubviews" | "renderState" | "onModeChange"> & {
  /**
   * Whether the initial mode should be `Hidden`.
   */
  initialHidden?: WithDefault<boolean, null> | undefined;
  /**
   * This was needed to get VirtualViewManagerDelegate to set this property.
   * TODO: Investigate why spread ViewProps doesn't call setter
   */
  removeClippedSubviews?: boolean | undefined;
  /**
   * Render state of children.
   *
   * - `0`: Reserved to represent unknown future values.
   * - `1`: Children are rendered.
   * - `2`: Children are not rendered.
   *
   * WORKAROUND: As of this writing, codegen doesn't support enums, so we need
   * to convert `number` into an enum in `VirtualView`.
   */
  renderState: Int32;
  /**
   * See `NativeModeChangeEvent`.
   */
  onModeChange?: DirectEventHandler<NativeModeChangeEvent> | undefined;
}>;
declare const $$VirtualViewNativeComponent: HostComponent<VirtualViewNativeProps>;
declare type $$VirtualViewNativeComponent = typeof $$VirtualViewNativeComponent;
export default $$VirtualViewNativeComponent;
