/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<32352c8f8040ea30be8f65d9d6e1dcb7>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\View\ViewNativeComponent.js
 */

import type { HostComponent } from "../../../src/private/types/HostComponent";
import type { HostInstance } from "../../../src/private/types/HostInstance";
import { type ViewProps as Props } from "./ViewPropTypes";
declare const ViewNativeComponent: HostComponent<Props>;
interface NativeCommands {
  readonly focus: (viewRef: HostInstance) => void;
  readonly blur: (viewRef: HostInstance) => void;
  readonly hotspotUpdate: (viewRef: HostInstance, x: number, y: number) => void;
  readonly setPressed: (viewRef: HostInstance, pressed: boolean) => void;
}
export declare const Commands: NativeCommands;
export declare type Commands = typeof Commands;
/**
 * `ViewNativeComponent` is an internal React Native host component, and is
 * exported to provide lower-level access for libraries.
 *
 * @warning `<unstable_NativeView>` provides no semver guarantees and is not
 *   intended to be used in app code. Please use
 *   [`<View>`](https://reactnative.dev/docs/view) instead.
 */
declare const $$ViewNativeComponent: typeof ViewNativeComponent;
declare type $$ViewNativeComponent = typeof $$ViewNativeComponent;
export default $$ViewNativeComponent;
