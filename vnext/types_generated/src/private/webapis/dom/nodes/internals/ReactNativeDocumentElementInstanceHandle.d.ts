/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<dd24350c9285cdda482727e49a0d178d>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\nodes\internals\ReactNativeDocumentElementInstanceHandle.js
 */

import type ReadOnlyNode from "../ReadOnlyNode";
import type { NativeElementReference } from "../specs/NativeDOM";
export declare type ReactNativeDocumentElementInstanceHandle = symbol & {
  __ReactNativeDocumentElementInstanceHandle__: string;
};
export declare function createReactNativeDocumentElementInstanceHandle(): ReactNativeDocumentElementInstanceHandle;
export declare function getNativeElementReferenceFromReactNativeDocumentElementInstanceHandle(instanceHandle: ReactNativeDocumentElementInstanceHandle): null | undefined | NativeElementReference;
export declare function setNativeElementReferenceForReactNativeDocumentElementInstanceHandle(instanceHandle: ReactNativeDocumentElementInstanceHandle, nativeElementReference: null | undefined | NativeElementReference): void;
export declare function getPublicInstanceFromReactNativeDocumentElementInstanceHandle(instanceHandle: ReactNativeDocumentElementInstanceHandle): null | undefined | ReadOnlyNode;
export declare function setPublicInstanceForReactNativeDocumentElementInstanceHandle(instanceHandle: ReactNativeDocumentElementInstanceHandle, publicInstance: null | undefined | ReadOnlyNode): void;
export declare function isReactNativeDocumentElementInstanceHandle(instanceHandle: unknown): instanceHandle is ReactNativeDocumentElementInstanceHandle;
