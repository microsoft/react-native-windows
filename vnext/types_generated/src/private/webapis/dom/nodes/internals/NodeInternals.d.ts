/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8b15d07d293319efc24eb799aac90387>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\nodes\internals\NodeInternals.js
 */

import type { InternalInstanceHandle } from "../../../../../../Libraries/Renderer/shims/ReactNativeTypes";
import type ReactNativeDocument from "../ReactNativeDocument";
import type ReadOnlyCharacterData from "../ReadOnlyCharacterData";
import type ReadOnlyElement from "../ReadOnlyElement";
import type ReadOnlyNode from "../ReadOnlyNode";
import type { NativeElementReference, NativeNodeReference, NativeTextReference } from "../specs/NativeDOM";
import type { ReactNativeDocumentElementInstanceHandle } from "./ReactNativeDocumentElementInstanceHandle";
import type { ReactNativeDocumentInstanceHandle } from "./ReactNativeDocumentInstanceHandle";
export type InstanceHandle = InternalInstanceHandle | ReactNativeDocumentElementInstanceHandle | ReactNativeDocumentInstanceHandle;
export declare function getInstanceHandle(node: ReadOnlyNode): InstanceHandle;
export declare function setInstanceHandle(node: ReadOnlyNode, instanceHandle: InstanceHandle): void;
export declare function getOwnerDocument(node: ReadOnlyNode): ReactNativeDocument | null;
export declare function setOwnerDocument(node: ReadOnlyNode, ownerDocument: ReactNativeDocument | null): void;
export declare function getPublicInstanceFromInstanceHandle(instanceHandle: InstanceHandle): null | undefined | ReadOnlyNode;
export declare function getNativeNodeReference(node: ReadOnlyNode): null | undefined | NativeNodeReference;
export declare function getNativeElementReference(node: ReadOnlyElement): null | undefined | NativeElementReference;
/**
 * Returns the current props for a node managed by React.
 * This accesses React internals (fiber.stateNode.canonical.currentProps).
 */
export declare function getCurrentProps(node: ReadOnlyNode): {
  [$$Key$$: string]: unknown;
};
export declare function getNativeTextReference(node: ReadOnlyCharacterData): null | undefined | NativeTextReference;
