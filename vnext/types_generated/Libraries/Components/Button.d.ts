/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<14ab920224fa9447c54cc26dcf41895b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Button.js
 */

import type { GestureResponderEvent } from "../Types/CoreEventTypes";
import type { AccessibilityActionEvent, AccessibilityActionInfo, AccessibilityState } from "./View/ViewAccessibility";
import { type ColorValue } from "../StyleSheet/StyleSheet";
import TouchableNativeFeedback from "./Touchable/TouchableNativeFeedback";
import TouchableOpacity from "./Touchable/TouchableOpacity";
import * as React from "react";
type ButtonPropsCore = Readonly<{
  /**
   * Text to display inside the button. On Android the given title will be
   * converted to the uppercased form.
   */
  title: string;
  /**
   * Handler called when the user taps the button.
   */
  onPress?: ((event?: GestureResponderEvent) => unknown) | undefined;
  /**
   * If `true`, doesn't play system sound on touch.
   *
   * @platform android
   *
   * @default `false`
   */
  touchSoundDisabled?: boolean | undefined;
  /**
   * Color of the text (iOS), or background color of the button (Android).
   *
   * @default {@platform android} `'#2196F3'`
   * @default {@platform ios} `'#007AFF'`
   */
  color?: ColorValue | undefined;
  /**
   * Designates the next view to receive focus when the user navigates down. See
   * the [Android documentation][android:nextFocusDown].
   *
   * [android:nextFocusDown]:
   * https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusDown
   *
   * @platform android, tv
   */
  nextFocusDown?: number | undefined;
  /**
   * Designates the next view to receive focus when the user navigates forward.
   * See the [Android documentation][android:nextFocusForward].
   *
   * [android:nextFocusForward]:
   * https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusForward
   *
   * @platform android, tv
   */
  nextFocusForward?: number | undefined;
  /**
   * Designates the next view to receive focus when the user navigates left. See
   * the [Android documentation][android:nextFocusLeft].
   *
   * [android:nextFocusLeft]:
   * https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusLeft
   *
   * @platform android, tv
   */
  nextFocusLeft?: number | undefined;
  /**
   * Designates the next view to receive focus when the user navigates right. See
   * the [Android documentation][android:nextFocusRight].
   *
   * [android:nextFocusRight]:
   * https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusRight
   *
   * @platform android, tv
   */
  nextFocusRight?: number | undefined;
  /**
   * Designates the next view to receive focus when the user navigates up. See
   * the [Android documentation][android:nextFocusUp].
   *
   * [android:nextFocusUp]:
   * https://developer.android.com/reference/android/view/View.html#attr_android:nextFocusUp
   *
   * @platform android, tv
   */
  nextFocusUp?: number | undefined;
  /**
   * Text to display for blindness accessibility features.
   */
  accessibilityLabel?: string | undefined;
  /**
   * Alias for `accessibilityLabel`.
   */
  "aria-label"?: string | undefined;
  /**
   * If `true`, disable all interactions for this component.
   *
   * @default `false`
   */
  disabled?: boolean | undefined;
  testID?: string | undefined;
  accessible?: boolean | undefined;
  accessibilityActions?: ReadonlyArray<AccessibilityActionInfo> | undefined;
  onAccessibilityAction?: ((event: AccessibilityActionEvent) => unknown) | undefined;
  accessibilityState?: AccessibilityState | undefined;
  /**
   * Alias for `accessibilityState`.
   */
  "aria-busy"?: boolean | undefined;
  "aria-checked"?: (boolean | undefined) | "mixed" | undefined;
  "aria-disabled"?: boolean | undefined;
  "aria-expanded"?: boolean | undefined;
  "aria-selected"?: boolean | undefined;
  importantForAccessibility?: ("auto" | "yes" | "no" | "no-hide-descendants") | undefined;
  accessibilityHint?: string | undefined;
  /**
   * A BCP 47 language tag for the screen reader to use when reading text
   * content.
   *
   * @platform ios
   */
  accessibilityLanguage?: string | undefined;
}>;
export interface ButtonProps extends ButtonPropsCore {}
declare const NativeTouchable: typeof TouchableNativeFeedback | typeof TouchableOpacity;
export type ButtonInstance = React.ComponentRef<typeof NativeTouchable>;
declare const Button: (props: Omit<ButtonProps, "ref"> & {
  ref?: React.Ref<ButtonInstance> | undefined;
}) => React.ReactNode;
/**
 * A basic button component that should render nicely on any platform. Supports a
 * minimal level of customization.
 *
 * If this button doesn't look right for your app, you can build your own button
 * using `Pressable`.
 *
 * Example:
 *
 * ```tsx
 * <Button
 *   onPress={onPressLearnMore}
 *   title="Learn More"
 *   color="#841584"
 *   accessibilityLabel="Learn more about this purple button"
 * />
 * ```
 *
 * @see https://reactnative.dev/docs/button
 */
declare const $$Button: typeof Button;
declare type $$Button = typeof $$Button;
export default $$Button;
