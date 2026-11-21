/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a6977a198bd08da86258bd525e1e29db>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Touchable\TouchableNativeFeedback.js
 */

import type { ProcessedColorValue } from "../../StyleSheet/processColor";
import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { TouchableWithoutFeedbackProps } from "./TouchableWithoutFeedback";
import Pressability from "../../Pressability/Pressability";
import * as React from "react";
type TouchableNativeFeedbackTVProps = {
  /**
   * Designates the next view to receive focus when the user navigates down. See the Android documentation.
   *
   * @platform android
   */
  nextFocusDown?: number | undefined;
  /**
   * Designates the next view to receive focus when the user navigates forward. See the Android documentation.
   *
   * @platform android
   */
  nextFocusForward?: number | undefined;
  /**
   * Designates the next view to receive focus when the user navigates left. See the Android documentation.
   *
   * @platform android
   */
  nextFocusLeft?: number | undefined;
  /**
   * Designates the next view to receive focus when the user navigates right. See the Android documentation.
   *
   * @platform android
   */
  nextFocusRight?: number | undefined;
  /**
   * Designates the next view to receive focus when the user navigates up. See the Android documentation.
   *
   * @platform android
   */
  nextFocusUp?: number | undefined;
};
export type TouchableNativeFeedbackProps = Readonly<Omit<TouchableWithoutFeedbackProps, keyof TouchableNativeFeedbackTVProps | "background" | "useForeground"> & Omit<TouchableNativeFeedbackTVProps, "background" | "useForeground"> & {
  /**
   * Determines the type of background drawable used to display touch feedback. Use one of the static methods to generate this value:
   *
   * - `TouchableNativeFeedback.SelectableBackground()` - Default background for selectable elements.
   * - `TouchableNativeFeedback.SelectableBackgroundBorderless()` - Default background for borderless selectable elements. API 21+.
   * - `TouchableNativeFeedback.Ripple(color, borderless)` - Ripple drawable with the specified color.
   */
  background?: (Readonly<{
    type: "ThemeAttrAndroid";
    attribute: "selectableItemBackground" | "selectableItemBackgroundBorderless";
    rippleRadius: number | undefined;
  }> | Readonly<{
    type: "RippleAndroid";
    color: ProcessedColorValue | undefined;
    borderless: boolean;
    rippleRadius: number | undefined;
  }>) | undefined;
  /**
   * If `true`, adds the ripple effect to the foreground of the view instead of the background. Useful if a child view has its own background, or you are displaying images.
   *
   * Check `TouchableNativeFeedback.canUseNativeForeground()` first, as this is only available on Android 6.0 and above. On older versions, this falls back to background.
   */
  useForeground?: boolean | undefined;
}>;
type TouchableNativeFeedbackState = Readonly<{
  pressability: Pressability;
}>;
/**
 * A wrapper for making views respond properly to touches (Android only). Uses native state drawable to display touch feedback.
 *
 * Supports only a single View instance as a child. If you need more extensive and future-proof touch handling, use `Pressable`.
 *
 * @see https://reactnative.dev/docs/touchablenativefeedback
 * @platform android
 */
declare class TouchableNativeFeedback extends React.Component<TouchableNativeFeedbackProps, TouchableNativeFeedbackState> {
  /**
   * Creates an object that represents android theme's default background for
   * selectable elements (?android:attr/selectableItemBackground).
   *
   * @param rippleRadius The radius of ripple effect
   */
  static SelectableBackground: (rippleRadius?: null | undefined | number) => Readonly<{
    attribute: "selectableItemBackground";
    type: "ThemeAttrAndroid";
    rippleRadius: number | undefined;
  }>;
  /**
   * Creates an object that represent android theme's default background for borderless
   * selectable elements (?android:attr/selectableItemBackgroundBorderless).
   * Available on android API level 21+.
   *
   * @param rippleRadius The radius of ripple effect
   */
  static SelectableBackgroundBorderless: (rippleRadius?: null | undefined | number) => Readonly<{
    attribute: "selectableItemBackgroundBorderless";
    type: "ThemeAttrAndroid";
    rippleRadius: number | undefined;
  }>;
  /**
   * Creates an object that represents ripple drawable with specified color (as a
   * string). If property `borderless` evaluates to true the ripple will
   * render outside of the view bounds (see native actionbar buttons as an
   * example of that behavior). This background type is available on Android
   * API level 21+.
   *
   * @param color The ripple color
   * @param borderless If the ripple can render outside it's bounds
   * @param rippleRadius The radius of ripple effect
   */
  static Ripple: (color: ColorValue, borderless: boolean, rippleRadius?: null | undefined | number) => Readonly<{
    borderless: boolean;
    color: ProcessedColorValue | undefined;
    rippleRadius: number | undefined;
    type: "RippleAndroid";
  }>;
  /**
   * Whether `useForeground` is supported.
   */
  static canUseNativeForeground: () => boolean;
  state: TouchableNativeFeedbackState;
  render(): React.ReactNode;
  componentDidUpdate(prevProps: TouchableNativeFeedbackProps, prevState: TouchableNativeFeedbackState): void;
  componentDidMount(): unknown;
  componentWillUnmount(): void;
}
export type TouchableNativeFeedbackInstance = TouchableNativeFeedback;
export default TouchableNativeFeedback;
