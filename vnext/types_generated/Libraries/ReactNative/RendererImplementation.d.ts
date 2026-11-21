/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<0f262dd57e31e87fa7d0863fdf6f51f5>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\RendererImplementation.js
 */

import type { RootTag } from "./RootTag";
import ReactFabric from "../Renderer/shims/ReactFabric";
import * as React from "react";
export declare function renderElement($$PARAM_0$$: {
  element: React.JSX.Element;
  rootTag: RootTag;
}): void;
export declare const dispatchCommand: typeof ReactFabric.dispatchCommand;
export declare type dispatchCommand = typeof dispatchCommand;
export declare const findHostInstance_DEPRECATED: typeof ReactFabric.findHostInstance_DEPRECATED;
export declare type findHostInstance_DEPRECATED = typeof findHostInstance_DEPRECATED;
export declare const findNodeHandle: typeof ReactFabric.findNodeHandle;
export declare type findNodeHandle = typeof findNodeHandle;
export declare const sendAccessibilityEvent: typeof ReactFabric.sendAccessibilityEvent;
export declare type sendAccessibilityEvent = typeof sendAccessibilityEvent;
export declare const isChildPublicInstance: typeof ReactFabric.isChildPublicInstance;
export declare type isChildPublicInstance = typeof isChildPublicInstance;
export declare const getNodeFromInternalInstanceHandle: typeof ReactFabric.getNodeFromInternalInstanceHandle;
export declare type getNodeFromInternalInstanceHandle = typeof getNodeFromInternalInstanceHandle;
export declare const getPublicInstanceFromInternalInstanceHandle: typeof ReactFabric.getPublicInstanceFromInternalInstanceHandle;
export declare type getPublicInstanceFromInternalInstanceHandle = typeof getPublicInstanceFromInternalInstanceHandle;
export declare const getPublicInstanceFromRootTag: typeof ReactFabric.getPublicInstanceFromRootTag;
export declare type getPublicInstanceFromRootTag = typeof getPublicInstanceFromRootTag;
export declare function isProfilingRenderer(): boolean;
