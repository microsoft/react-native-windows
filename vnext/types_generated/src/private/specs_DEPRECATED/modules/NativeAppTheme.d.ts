/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<97656147a1a92ad1a5660e817c951c01>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\specs_DEPRECATED\modules\NativeAppTheme.js
 */

import type { TurboModule } from "../../../../Libraries/TurboModule/RCTExport";
export type HighContrastColors = {
  ButtonFaceColor: string;
  ButtonTextColor: string;
  GrayTextColor: string;
  HighlightColor: string;
  HighlightTextColor: string;
  HotlightColor: string;
  WindowColor: string;
  WindowTextColor: string;
};
export type IHighContrastChangedEvent = {
  isHighContrast: boolean;
  highContrastColors: HighContrastColors;
};
export interface Spec extends TurboModule {
  readonly getConstants: () => IHighContrastChangedEvent;
}
declare const $$NativeAppTheme: null | undefined | Spec;
declare type $$NativeAppTheme = typeof $$NativeAppTheme;
export default $$NativeAppTheme;
