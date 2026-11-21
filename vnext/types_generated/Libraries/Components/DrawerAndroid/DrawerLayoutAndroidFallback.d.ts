/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<0fdd5c7254af76d7e6e4c01624d3935c>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\DrawerAndroid\DrawerLayoutAndroidFallback.js
 */

import type { MeasureInWindowOnSuccessCallback, MeasureLayoutOnSuccessCallback, MeasureOnSuccessCallback } from "../../../src/private/types/HostInstance";
import type { DrawerLayoutAndroidMethods, DrawerLayoutAndroidProps, DrawerLayoutAndroidState } from "./DrawerLayoutAndroidTypes";
import * as React from "react";
declare class DrawerLayoutAndroid extends React.Component<DrawerLayoutAndroidProps, DrawerLayoutAndroidState> implements DrawerLayoutAndroidMethods {
  render(): React.ReactNode;
  openDrawer(): void;
  closeDrawer(): void;
  blur(): void;
  focus(): void;
  measure(callback: MeasureOnSuccessCallback): void;
  measureInWindow(callback: MeasureInWindowOnSuccessCallback): void;
  measureLayout(relativeToNativeNode: number, onSuccess: MeasureLayoutOnSuccessCallback, onFail?: () => void): void;
  setNativeProps(nativeProps: Object): void;
}
export default DrawerLayoutAndroid;
