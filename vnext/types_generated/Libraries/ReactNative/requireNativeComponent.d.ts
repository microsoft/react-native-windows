/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a547749dfe7cf6a4fe1d84dde49fc303>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\requireNativeComponent.js
 */

import type { HostComponent } from "../../src/private/types/HostComponent";
declare const requireNativeComponent: <T extends {}>(uiViewClassName: string) => HostComponent<T>;
/**
 * Creates values that can be used like React components which represent native
 * view managers. You should create JavaScript modules that wrap these values so
 * that the results are memoized. Example:
 *
 *   const View = requireNativeComponent('RCTView');
 *
 */
declare const $$requireNativeComponent: typeof requireNativeComponent;
declare type $$requireNativeComponent = typeof $$requireNativeComponent;
export default $$requireNativeComponent;
