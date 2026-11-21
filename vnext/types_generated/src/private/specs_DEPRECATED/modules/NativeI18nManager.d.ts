/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<15d26bb4a709a5d2f7e684b0eae659e1>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\specs_DEPRECATED\modules\NativeI18nManager.js
 */

import type { TurboModule } from "../../../../Libraries/TurboModule/RCTExport";
export type I18nManagerConstants = {
  doLeftAndRightSwapInRTL: boolean;
  isRTL: boolean;
  localeIdentifier?: string | undefined;
};
export interface Spec extends TurboModule {
  readonly getConstants: () => I18nManagerConstants;
  allowRTL: (allowRTL: boolean) => void;
  forceRTL: (forceRTL: boolean) => void;
  swapLeftAndRightInRTL: (flipStyles: boolean) => void;
}
declare const $$NativeI18nManager: null | undefined | Spec;
declare type $$NativeI18nManager = typeof $$NativeI18nManager;
export default $$NativeI18nManager;
