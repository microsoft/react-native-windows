/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8c56115b8017098f173780ca11a162d3>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\nodes\internals\ReactNativeDocumentInstanceHandle.js
 */

import type { RootTag } from "../../../../../../Libraries/ReactNative/RootTag";
import type ReactNativeDocument from "../ReactNativeDocument";
import type { NativeNodeReference } from "../specs/NativeDOM";
export declare type ReactNativeDocumentInstanceHandle = symbol & {
  __ReactNativeDocumentInstanceHandle__: string;
};
export declare function createReactNativeDocumentInstanceHandle(rootTag: RootTag): ReactNativeDocumentInstanceHandle;
export declare function getNativeNodeReferenceFromReactNativeDocumentInstanceHandle(instanceHandle: ReactNativeDocumentInstanceHandle): null | undefined | NativeNodeReference;
export declare function getPublicInstanceFromReactNativeDocumentInstanceHandle(instanceHandle: ReactNativeDocumentInstanceHandle): null | undefined | ReactNativeDocument;
export declare function isReactNativeDocumentInstanceHandle(instanceHandle: unknown): instanceHandle is ReactNativeDocumentInstanceHandle;
