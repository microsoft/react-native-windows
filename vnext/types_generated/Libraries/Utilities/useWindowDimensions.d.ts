/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<e0d231b0ac39284090fd75c50e2c29dc>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Utilities\useWindowDimensions.js
 */

import { type DisplayMetrics, type DisplayMetricsAndroid } from "./NativeDeviceInfo";
/**
 * React hook that provides the application window's width, height, scale, and
 * font scale. Automatically updates when screen size or font scale changes.
 *
 * @see https://reactnative.dev/docs/usewindowdimensions
 */
declare function useWindowDimensions(): DisplayMetrics | DisplayMetricsAndroid;
export default useWindowDimensions;
