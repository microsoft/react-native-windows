/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a8506bb94047dc03d5647b692714d2bb>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Pressable\useAndroidRippleForView.js
 */

import type { ProcessedColorValue } from "../../StyleSheet/processColor";
import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { GestureResponderEvent } from "../../Types/CoreEventTypes";
import View from "../View/View";
import * as React from "react";
type NativeBackgroundProp = Readonly<{
  type: "RippleAndroid";
  color: ProcessedColorValue | undefined;
  borderless: boolean;
  rippleRadius: number | undefined;
  alpha: number | undefined;
}>;
export type PressableAndroidRippleConfig = {
  color?: ColorValue | undefined;
  borderless?: boolean | undefined;
  radius?: number | undefined;
  foreground?: boolean | undefined;
  alpha?: number | undefined;
};
/**
 * Provides the event handlers and props for configuring the ripple effect on
 * supported versions of Android.
 */
declare function useAndroidRippleForView(rippleConfig: null | undefined | PressableAndroidRippleConfig, viewRef: {
  current: null | React.ComponentRef<typeof View>;
}): null | undefined | Readonly<{
  onPressIn: (event: GestureResponderEvent) => void;
  onPressMove: (event: GestureResponderEvent) => void;
  onPressOut: (event: GestureResponderEvent) => void;
  viewProps: Readonly<{
    nativeBackgroundAndroid: NativeBackgroundProp;
  }> | Readonly<{
    nativeForegroundAndroid: NativeBackgroundProp;
  }>;
}>;
export default useAndroidRippleForView;
