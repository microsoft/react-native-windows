/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<0ef0f33e262b819bac71e25adfb877fd>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Touchable\TouchableWithoutFeedback.js
 */

import type { AccessibilityActionEvent } from "../../Components/View/ViewAccessibility";
import type { EdgeInsetsOrSizeProp } from "../../StyleSheet/EdgeInsetsPropType";
import type { BlurEvent, FocusEvent, GestureResponderEvent, LayoutChangeEvent } from "../../Types/CoreEventTypes";
import { type AccessibilityProps } from "../../Components/View/ViewAccessibility";
import { type ViewStyleProp } from "../../StyleSheet/StyleSheet";
import * as React from "react";
export type TouchableWithoutFeedbackPropsIOS = {};
export type TouchableWithoutFeedbackPropsAndroid = {
  /**
   * If true, doesn't play a system sound on touch.
   *
   * @platform android
   */
  touchSoundDisabled?: boolean | undefined;
};
type TouchableWithoutFeedbackPropsCore = {
  children?: React.ReactNode | undefined;
  /**
   * Delay in ms, from onPressIn, before onLongPress is called.
   */
  delayLongPress?: number | undefined;
  /**
   * Delay in ms, from the start of the touch, before onPressIn is called.
   */
  delayPressIn?: number | undefined;
  /**
   * Delay in ms, from the release of the touch, before onPressOut is called.
   */
  delayPressOut?: number | undefined;
  /**
   * If true, disable all interactions for this component.
   */
  disabled?: boolean | undefined;
  /**
   * Whether this View should be focusable with a non-touch input device,
   * eg. receive focus with a hardware keyboard / TV remote.
   */
  focusable?: boolean | undefined;
  /**
   * This defines how far your touch can start away from the button.
   * This is added to pressRetentionOffset when moving off of the button.
   * NOTE The touch area never extends past the parent view bounds and
   * the Z-index of sibling views always takes precedence if a touch hits
   * two overlapping views.
   */
  hitSlop?: EdgeInsetsOrSizeProp | undefined;
  /**
   * Used to reference react managed views from native code.
   */
  id?: string | undefined;
  importantForAccessibility?: ("auto" | "yes" | "no" | "no-hide-descendants") | undefined;
  nativeID?: string | undefined;
  onAccessibilityAction?: ((event: AccessibilityActionEvent) => unknown) | undefined;
  /**
   * When `accessible` is true (which is the default) this may be called when
   * the OS-specific concept of "blur" occurs, meaning the element lost focus.
   * Some platforms may not have the concept of blur.
   */
  onBlur?: ((event: BlurEvent) => unknown) | undefined;
  /**
   * When `accessible` is true (which is the default) this may be called when
   * the OS-specific concept of "focus" occurs. Some platforms may not have
   * the concept of focus.
   */
  onFocus?: ((event: FocusEvent) => unknown) | undefined;
  /**
   * Invoked on mount and layout changes with
   * {nativeEvent: {layout: {x, y, width, height}}}
   */
  onLayout?: ((event: LayoutChangeEvent) => unknown) | undefined;
  onLongPress?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called when the touch is released,
   * but not if cancelled (e.g. by a scroll that steals the responder lock).
   */
  onPress?: ((event: GestureResponderEvent) => unknown) | undefined;
  onPressIn?: ((event: GestureResponderEvent) => unknown) | undefined;
  onPressOut?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * When the scroll view is disabled, this defines how far your
   * touch may move off of the button, before deactivating the button.
   * Once deactivated, try moving it back and you'll see that the button
   * is once again reactivated! Move it back and forth several times
   * while the scroll view is disabled. Ensure you pass in a constant
   * to reduce memory allocations.
   */
  pressRetentionOffset?: EdgeInsetsOrSizeProp | undefined;
  rejectResponderTermination?: boolean | undefined;
  /**
   * Used to locate this view in end-to-end tests.
   */
  testID?: string | undefined;
  /**
   * //FIXME: not in doc but available in examples
   */
  style?: ViewStyleProp | undefined;
};
export interface TouchableWithoutFeedbackProps extends Readonly<TouchableWithoutFeedbackPropsCore & TouchableWithoutFeedbackPropsAndroid & TouchableWithoutFeedbackPropsIOS & AccessibilityProps> {}
/**
 * Do not use unless you have a very good reason. All elements that respond to press should have a visual feedback when touched.
 *
 * Supports only one child. If you need more extensive and future-proof touch handling, use `Pressable`.
 *
 * @see https://reactnative.dev/docs/touchablewithoutfeedback
 */
declare function TouchableWithoutFeedback(props: TouchableWithoutFeedbackProps): React.ReactNode;
export default TouchableWithoutFeedback;
