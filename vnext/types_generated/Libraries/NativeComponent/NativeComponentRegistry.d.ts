/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<f5d0e18b91735228c2d23b51e08c4af6>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\NativeComponent\NativeComponentRegistry.js
 */

import type { HostComponent } from "../../src/private/types/HostComponent";
import type { PartialViewConfig } from "../Renderer/shims/ReactNativeTypes";
import * as React from "react";
/**
 * Configures a function that is called to determine whether a given component
 * should be registered using reflection of the native component at runtime.
 *
 * The provider should return null if the native component is unavailable in
 * the current environment.
 */
export declare function setRuntimeConfigProvider(runtimeConfigProvider: (name: string) => null | undefined | {
  native: boolean;
  verify: boolean;
}): void;
/**
 * Gets a `NativeComponent` that can be rendered by React Native.
 *
 * The supplied `viewConfigProvider` may or may not be invoked and utilized,
 * depending on how `setRuntimeConfigProvider` is configured.
 */
export declare function get<Config extends {}>(name: string, viewConfigProvider: () => PartialViewConfig): HostComponent<Config>;
/**
 * Same as `NativeComponentRegistry.get(...)`, except this will check either
 * the `setRuntimeConfigProvider` configuration or use native reflection (slow)
 * to determine whether this native component is available.
 *
 * If the native component is not available, a stub component is returned. Note
 * that the return value of this is not `HostComponent` because the returned
 * component instance is not guaranteed to have native methods.
 */
export declare function getWithFallback_DEPRECATED<Config extends {}>(name: string, viewConfigProvider: () => PartialViewConfig): React.ComponentType<Config>;
/**
 * Unstable API. Do not use!
 *
 * This method returns if there is a StaticViewConfig registered for the
 * component name received as a parameter.
 */
export declare function unstable_hasStaticViewConfig(name: string): boolean;
