/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<c9e112427aa825664292eeaa858fafbf>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\components\virtualview\VirtualView.js
 */

import type { ViewStyleProp } from "../../../../Libraries/StyleSheet/StyleSheet";
import type { HostInstance } from "../../types/HostInstance";
import type { NativeModeChangeEvent } from "./VirtualViewNativeComponent";
import VirtualViewExperimentalNativeComponent from "./VirtualViewExperimentalNativeComponent";
import * as React from "react";
export declare enum VirtualViewMode {
  Visible = 0,
  Prerender = 1,
  Hidden = 2,
}
export declare namespace VirtualViewMode {
  export function cast(value: number | null | undefined): VirtualViewMode;
  export function isValid(value: number | null | undefined): value is VirtualViewMode;
  export function members(): IterableIterator<VirtualViewMode>;
  export function getName(value: VirtualViewMode): string;
}
export declare enum VirtualViewRenderState {
  Unknown = 0,
  Rendered = 1,
  None = 2,
}
export declare namespace VirtualViewRenderState {
  export function cast(value: number | null | undefined): VirtualViewRenderState;
  export function isValid(value: number | null | undefined): value is VirtualViewRenderState;
  export function members(): IterableIterator<VirtualViewRenderState>;
  export function getName(value: VirtualViewRenderState): string;
}
export type Rect = Readonly<{
  x: number;
  y: number;
  width: number;
  height: number;
}>;
export type ModeChangeEvent = Readonly<Omit<Omit<NativeModeChangeEvent, "mode">, "renderState" | "mode" | "target"> & {
  renderState: VirtualViewRenderState;
  mode: VirtualViewMode;
  target: HostInstance;
}>;
declare const VirtualViewNativeComponent: typeof VirtualViewExperimentalNativeComponent;
type VirtualViewComponent = (props: {
  children?: React.ReactNode | undefined;
  hiddenStyle?: ((targetRect: Rect) => ViewStyleProp) | undefined;
  nativeID?: string | undefined;
  ref?: null | undefined | React.Ref<React.ComponentRef<typeof VirtualViewNativeComponent>>;
  style?: null | undefined | ViewStyleProp;
  onModeChange?: ((event: ModeChangeEvent) => void) | undefined;
  removeClippedSubviews?: boolean | undefined;
}) => React.ReactNode;
declare const NotHidden: null;
type HiddenStyle = Exclude<ViewStyleProp, typeof NotHidden>;
type State = HiddenStyle | typeof NotHidden;
declare const $$VirtualView: VirtualViewComponent;
declare type $$VirtualView = typeof $$VirtualView;
export default $$VirtualView;
export declare function createHiddenVirtualView(style: ViewStyleProp): VirtualViewComponent;
export declare const _logs: {
  states?: Array<State> | undefined;
};
export declare type _logs = typeof _logs;
