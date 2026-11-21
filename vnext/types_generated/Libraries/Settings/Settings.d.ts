/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<fcd478402b0b2fb336d1869b22f4d8b3>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Settings\Settings.js
 */

declare let Settings: {
  get<T = unknown>(key: string): T | undefined;
  set(settings: {
    [$$Key$$: string]: unknown;
  }): void;
  watchKeys(keys: string | Array<string>, callback: () => void): number;
  clearWatch(watchId: number): void;
};
declare const $$Settings: typeof Settings;
declare type $$Settings = typeof $$Settings;
export default $$Settings;
