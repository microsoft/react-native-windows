/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<148a4fedde46e2e7924411a25b64d90b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Pressable\Pressable.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { ViewStyleProp } from "../../StyleSheet/StyleSheet";
import type { GestureResponderEvent, LayoutChangeEvent, MouseEvent } from "../../Types/CoreEventTypes";
import type { ViewProps } from "../View/ViewPropTypes";
import { type RectOrSize } from "../../StyleSheet/Rect";
import { type PressableAndroidRippleConfig } from "./useAndroidRippleForView";
import * as React from "react";
export type PressableInstance = HostInstance;
export type { PressableAndroidRippleConfig };
type PressableStateCallbackTypeCore = Readonly<{
  pressed: boolean;
}>;
export interface PressableStateCallbackType extends PressableStateCallbackTypeCore {}
type PressableBaseProps = Readonly<{
  /**
   * Whether a press gesture can be interrupted by a parent gesture such as a
   * scroll event. Defaults to true.
   */
  cancelable?: boolean | undefined;
  /**
   * Either children or a function that receives a boolean reflecting whether
   * the component is currently pressed.
   */
  children?: React.ReactNode | ((state: PressableStateCallbackType) => React.ReactNode) | undefined;
  /**
   * Duration to wait after hover in before calling `onHoverIn`.
   *
   * @platform macos windows
   */
  delayHoverIn?: number | undefined;
  /**
   * Duration to wait after hover out before calling `onHoverOut`.
   *
   * @platform macos windows
   */
  delayHoverOut?: number | undefined;
  /**
   * Duration (in milliseconds) from `onPressIn` before `onLongPress` is called.
   *
   * @default `500`
   */
  delayLongPress?: number | undefined;
  /**
   * Whether the press behavior is disabled.
   *
   * @default `false`
   */
  disabled?: boolean | undefined;
  /**
   * Additional distance outside of this view in which a press is detected.
   */
  hitSlop?: RectOrSize | undefined;
  /**
   * Additional distance outside of this view in which a touch is considered a
   * press before `onPressOut` is triggered.
   *
   * @default `{bottom: 30, left: 20, right: 20, top: 20}`
   */
  pressRetentionOffset?: RectOrSize | undefined;
  /**
   * Called when this view's layout changes.
   */
  onLayout?: ((event: LayoutChangeEvent) => unknown) | undefined;
  /**
   * Called when the hover is activated.
   */
  onHoverIn?: ((event: MouseEvent) => unknown) | undefined;
  /**
   * Called when the hover is deactivated.
   */
  onHoverOut?: ((event: MouseEvent) => unknown) | undefined;
  /**
   * Called if the time after `onPressIn` lasts longer than `delayLongPress`.
   */
  onLongPress?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called after `onPressOut`.
   */
  onPress?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called immediately when a touch is engaged, before `onPressOut` and
   * `onPress`.
   */
  onPressIn?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called when the press location moves.
   */
  onPressMove?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Called when a touch is released.
   */
  onPressOut?: ((event: GestureResponderEvent) => unknown) | undefined;
  /**
   * Whether to prevent any other native components from becoming responder
   * while this pressable is responder.
   */
  blockNativeResponder?: boolean | undefined;
  /**
   * Either view styles or a function that receives a boolean reflecting whether
   * the component is currently pressed and returns view styles.
   */
  style?: ViewStyleProp | ((state: PressableStateCallbackType) => ViewStyleProp) | undefined;
  /**
   * Identifier used to find this view in tests.
   */
  testID?: string | undefined;
  /**
   * If true, doesn't play Android system sound on press.
   *
   * @platform android
   * @default `false`
   */
  android_disableSound?: boolean | undefined;
  /**
   * Enables the Android ripple effect and configures its properties.
   *
   * @platform android
   */
  android_ripple?: PressableAndroidRippleConfig | undefined;
  /**
   * Used only for documentation or testing (e.g. snapshot testing).
   */
  testOnly_pressed?: boolean | undefined;
  /**
   * Duration (in milliseconds) to wait after press down before calling
   * `onPressIn`.
   */
  unstable_pressDelay?: number | undefined;
}>;
export interface PressableProps extends Readonly<Omit<Omit<ViewProps, "onMouseEnter" | "onMouseLeave">, keyof PressableBaseProps | keyof {}> & Omit<PressableBaseProps, keyof {}>> {}
declare const MemoedPressable: (props: Omit<PressableProps, "ref"> & {
  ref?: React.Ref<PressableInstance> | undefined;
}) => React.ReactNode;
/**
 * A Core Component wrapper that can detect various stages of press
 * interactions on any of its defined children.
 *
 * @see https://reactnative.dev/docs/pressable
 */
declare const $$Pressable: typeof MemoedPressable;
declare type $$Pressable = typeof $$Pressable;
export default $$Pressable;
