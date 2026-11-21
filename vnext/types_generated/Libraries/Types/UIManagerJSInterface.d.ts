/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<b42c60456cc85abc90238dee42e40853>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Types\UIManagerJSInterface.js
 */

import type { Spec, ViewManagerConfig } from "../ReactNative/NativeUIManager";
export interface UIManagerJSInterface extends Spec {
  readonly getViewManagerConfig: (viewManagerName: string) => ViewManagerConfig;
  readonly hasViewManagerConfig: (viewManagerName: string) => boolean;
}
