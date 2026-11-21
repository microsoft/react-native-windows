/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<be6fe23ab8c5706aa0b4f738072c57c5>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\RootTag.js
 */

import * as React from "react";
export declare type RootTag = symbol & {
  __RootTag__: string;
};
export declare const RootTagContext: React.Context<RootTag>;
export declare type RootTagContext = typeof RootTagContext;
/**
 * Intended to only be used by `AppContainer`.
 */
export declare function createRootTag(rootTag: number | RootTag): RootTag;
