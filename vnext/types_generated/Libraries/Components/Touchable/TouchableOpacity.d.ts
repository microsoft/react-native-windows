/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<4de38002e52c2d69672a97d2d44a1fa8>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Touchable\TouchableOpacity.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { ViewStyleProp } from "../../StyleSheet/StyleSheet";
import type { TouchableWithoutFeedbackProps } from "./TouchableWithoutFeedback";
import Animated from "../../Animated/Animated";
import * as React from "react";
export type TouchableOpacityInstance = HostInstance;
export type TouchableOpacityTVProps = Readonly<{
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
}>;
type TouchableOpacityBaseProps = Readonly<{
  /**
   * Opacity of the wrapped view when touch is active.
   *
   * @default `0.2`
   */
  activeOpacity?: number | undefined;
  style?: Animated.WithAnimatedValue<ViewStyleProp> | undefined;
  hostRef?: React.Ref<TouchableOpacityInstance> | undefined;
}>;
export type TouchableOpacityProps = Readonly<Omit<TouchableWithoutFeedbackProps, keyof TouchableOpacityTVProps | keyof TouchableOpacityBaseProps | keyof {}> & Omit<TouchableOpacityTVProps, keyof TouchableOpacityBaseProps | keyof {}> & Omit<TouchableOpacityBaseProps, keyof {}> & {}>;
declare const Touchable: (props: Omit<TouchableOpacityProps, "ref"> & {
  ref?: React.Ref<TouchableOpacityInstance> | undefined;
}) => React.ReactNode;
/**
 * A wrapper for making views respond properly to touches. On press down, the opacity of the wrapped view is decreased, dimming it.
 *
 * Opacity is controlled by wrapping the children in an `Animated.View`, which is added to the view hierarchy. Be aware that this can affect layout.
 *
 * If you need more extensive and future-proof touch handling, use `Pressable`.
 *
 * @see https://reactnative.dev/docs/touchableopacity
 */
declare const $$TouchableOpacity: typeof Touchable;
declare type $$TouchableOpacity = typeof $$TouchableOpacity;
export default $$TouchableOpacity;
