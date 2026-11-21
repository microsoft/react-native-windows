/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<819a16c12353bde32f75dca6841aa5e7>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\specs_DEPRECATED\modules\NativeDeviceInfo.js
 */

import type { TurboModule } from "../../../../Libraries/TurboModule/RCTExport";
export type DisplayMetricsAndroid = {
  width: number;
  height: number;
  scale: number;
  fontScale: number;
  densityDpi: number;
};
export type DisplayMetrics = {
  width: number;
  height: number;
  scale: number;
  fontScale: number;
};
export type DimensionsPayload = {
  window?: DisplayMetrics | undefined;
  screen?: DisplayMetrics | undefined;
  windowPhysicalPixels?: DisplayMetricsAndroid | undefined;
  screenPhysicalPixels?: DisplayMetricsAndroid | undefined;
};
export type DeviceInfoConstants = {
  readonly Dimensions: DimensionsPayload;
  readonly isEdgeToEdge?: boolean | undefined;
  readonly isIPhoneX_deprecated?: boolean | undefined;
};
export interface Spec extends TurboModule {
  readonly getConstants: () => DeviceInfoConstants;
}
declare const NativeModule: Spec;
declare const $$NativeDeviceInfo: typeof NativeModule;
declare type $$NativeDeviceInfo = typeof $$NativeDeviceInfo;
export default $$NativeDeviceInfo;
