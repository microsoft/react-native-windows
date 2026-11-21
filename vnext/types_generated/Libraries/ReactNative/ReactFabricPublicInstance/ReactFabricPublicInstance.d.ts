/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<d55c043952e89176dcc056be8168ffc0>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\ReactFabricPublicInstance\ReactFabricPublicInstance.js
 */

/**
 * This module is meant to be used by the React renderers to create public
 * instances and get some data from them (like their instance handle / fiber).
 */

import type { InternalInstanceHandle, Node, ViewConfig } from "../../Renderer/shims/ReactNativeTypes";
import type { RootTag } from "../RootTag";
import ReactNativeDocument from "../../../src/private/webapis/dom/nodes/ReactNativeDocument";
import ReactNativeElement from "../../../src/private/webapis/dom/nodes/ReactNativeElement";
import ReadOnlyText from "../../../src/private/webapis/dom/nodes/ReadOnlyText";
export declare type PublicRootInstance = symbol & {
  __PublicRootInstance__: string;
};
export declare function createPublicRootInstance(rootTag: RootTag): PublicRootInstance;
export declare function createPublicInstance(tag: number, viewConfig: ViewConfig, internalInstanceHandle: InternalInstanceHandle, ownerDocument: ReactNativeDocument): ReactNativeElement;
export declare function createPublicTextInstance(internalInstanceHandle: InternalInstanceHandle, ownerDocument: ReactNativeDocument): ReadOnlyText;
export type PublicTextInstance = ReturnType<typeof createPublicTextInstance>;
export declare function getNativeTagFromPublicInstance(publicInstance: ReactNativeElement): number;
export declare function getNodeFromPublicInstance(publicInstance: ReactNativeElement): null | undefined | Node;
export declare function getInternalInstanceHandleFromPublicInstance(publicInstance: ReactNativeElement): InternalInstanceHandle;
