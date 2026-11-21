/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<4c167c970dc750140cdc5f5589ee3d0e>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\ReactNative\DisplayMode.js
 */

export declare type DisplayModeType = symbol & {
  __DisplayModeType__: string;
};
declare const DisplayMode: {
  readonly [$$Key$$: string]: DisplayModeType;
};
export declare function coerceDisplayMode(value: null | undefined | number): DisplayModeType;
/** DisplayMode should be in sync with the method displayModeToInt from
 * react/renderer/uimanager/primitives.h. */
declare const $$DisplayMode: typeof DisplayMode;
declare type $$DisplayMode = typeof $$DisplayMode;
export default $$DisplayMode;
