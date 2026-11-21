/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<35854a6e0ce8a23d55c9064c05c7152c>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Text\TextProps.js
 */

import type { AccessibilityActionEvent, AccessibilityProps, Role } from "../Components/View/ViewAccessibility";
import type { ColorValue, TextStyleProp } from "../StyleSheet/StyleSheet";
import type { GestureResponderEvent, LayoutChangeEvent, PointerEvent, TextLayoutEvent } from "../Types/CoreEventTypes";
import * as React from "react";
export type PressRetentionOffset = Readonly<{
  top: number;
  left: number;
  bottom: number;
  right: number;
}>;
type TextPointerEventProps = Readonly<{
  onPointerEnter?: ((event: PointerEvent) => void) | undefined;
  onPointerLeave?: ((event: PointerEvent) => void) | undefined;
  onPointerMove?: ((event: PointerEvent) => void) | undefined;
}>;
export type TextPropsIOS = {
  /**
   * Whether fonts should be scaled down to fit style constraints.
   *
   * @default `false`
   * @platform ios
   */
  adjustsFontSizeToFit?: boolean | undefined;
  /**
   * The [Dynamic Type](https://developer.apple.com/documentation/uikit/uifont/textstyle)
   * ramp to apply.
   *
   * @default `'body'`
   * @platform ios
   */
  dynamicTypeRamp?: ("caption2" | "caption1" | "footnote" | "subheadline" | "callout" | "body" | "headline" | "title3" | "title2" | "title1" | "largeTitle") | undefined;
  /**
   * When `true`, no visual change is made when text is pressed down. By
   * default, a gray oval highlights the text on press down.
   *
   * @default `false`
   * @platform ios
   */
  suppressHighlighting?: boolean | undefined;
  /**
   * Line break strategy on iOS.
   *
   * @default `'none'`
   * @platform ios
   */
  lineBreakStrategyIOS?: ("none" | "standard" | "hangul-word" | "push-out") | undefined;
};
export type TextPropsAndroid = {
  /**
   * Specifies the disabled state of the text view for testing purposes.
   *
   * @platform android
   */
  disabled?: boolean | undefined;
  /**
   * Highlight color of the text when selected.
   *
   * @platform android
   */
  selectionColor?: ColorValue | undefined;
  /**
   * Types of data converted to clickable URLs in the text element.
   *
   * @default `'none'`
   * @platform android
   */
  dataDetectorType?: ("phoneNumber" | "link" | "email" | "none" | "all") | undefined;
  /**
   * Text break strategy on Android.
   *
   * @default `'highQuality'`
   * @platform android
   */
  textBreakStrategy?: ("balanced" | "highQuality" | "simple") | undefined;
  adjustsFontSizeToFit?: boolean | undefined;
  /**
   * Smallest possible font scale when `adjustsFontSizeToFit` is enabled
   * (values 0.01-1.0).
   */
  minimumFontScale?: number | undefined;
};
type TextBaseProps = Readonly<{
  onAccessibilityAction?: ((event: AccessibilityActionEvent) => unknown) | undefined;
  accessibilityLevel?: number | undefined;
  accessibilityPosInSet?: number | undefined;
  accessibilitySetSize?: number | undefined;
  tooltip?: string | undefined;
  /**
   * Controls whether the `Text` can be the target of touch events.
   */
  pointerEvents?: ("auto" | "box-none" | "box-only" | "none") | undefined;
  /**
   * Whether fonts should scale to respect Text Size accessibility settings.
   *
   * @default `true`
   */
  allowFontScaling?: boolean | undefined;
  /**
   * Sets automatic hyphenation frequency.
   *
   * @default `'none'`
   * @platform android
   */
  android_hyphenationFrequency?: ("normal" | "none" | "full") | undefined;
  /**
   * Sets the strictness of the line-breaking rules, such as which characters
   * may start or end a line in Chinese and Japanese text (kinsoku), as CSS
   * `line-break`. Currently Android only, and ignored below Android 13 (API
   * level 33).
   *
   * @default `'auto'`
   * @platform android
   */
  lineBreak?: ("auto" | "loose" | "normal" | "strict") | undefined;
  /**
   * Sets word-based line breaking, as CSS `word-break`. `'auto-phrase'` breaks
   * Japanese text at phrase boundaries instead of between any two characters.
   * Currently Android only, and ignored below Android 13 (API level 33).
   *
   * Support depends on the locale, and text is laid out in the device locale:
   * where that locale has no phrase-based breaking, `'auto-phrase'` has no
   * effect.
   *
   * @default `'normal'`
   * @platform android
   */
  wordBreak?: ("normal" | "auto-phrase") | undefined;
  "aria-posinset"?: number | undefined;
  "aria-setsize"?: number | undefined;
  "aria-level"?: number | undefined;
  "aria-readonly"?: boolean | undefined;
  "aria-multiselectable"?: boolean | undefined;
  "aria-required"?: boolean | undefined;
  children?: React.ReactNode | undefined;
  /**
   * How text is truncated when `numberOfLines` is set. On Android with
   * `numberOfLines` greater than 1, only `'tail'` works correctly.
   *
   * @default `'tail'`
   */
  ellipsizeMode?: ("clip" | "head" | "middle" | "tail") | undefined;
  id?: string | undefined;
  /**
   * Largest possible font scale when `allowFontScaling` is enabled.
   * `null`/`undefined` inherits from the parent node or the global default (0).
   * `0` means no max (ignores parent/global default). `>= 1` sets the
   * `maxFontSizeMultiplier` of this node to this value.
   */
  maxFontSizeMultiplier?: number | undefined;
  nativeID?: string | undefined;
  /**
   * Truncate text with an ellipsis after this many lines. `0` means no
   * restriction.
   *
   * @default `0`
   */
  numberOfLines?: number | undefined;
  onLayout?: ((event: LayoutChangeEvent) => unknown) | undefined;
  /** Called on long press. */
  onLongPress?: ((event: GestureResponderEvent) => unknown) | undefined;
  /** Called on press, triggered after `onPressOut`. */
  onPress?: ((event: GestureResponderEvent) => unknown) | undefined;
  /** Called immediately when a touch is engaged. */
  onPressIn?: ((event: GestureResponderEvent) => unknown) | undefined;
  /** Called when a touch is released. */
  onPressOut?: ((event: GestureResponderEvent) => unknown) | undefined;
  onResponderGrant?: ((event: GestureResponderEvent) => void) | undefined;
  onResponderMove?: ((event: GestureResponderEvent) => void) | undefined;
  onResponderRelease?: ((event: GestureResponderEvent) => void) | undefined;
  onResponderTerminate?: ((event: GestureResponderEvent) => void) | undefined;
  onResponderTerminationRequest?: (() => boolean) | undefined;
  onStartShouldSetResponder?: (() => boolean) | undefined;
  onMoveShouldSetResponder?: (() => boolean) | undefined;
  /** Invoked on text layout change. */
  onTextLayout?: ((event: TextLayoutEvent) => unknown) | undefined;
  /**
   * Defines how far your touch may move off of the button, before
   * deactivating the button.
   */
  pressRetentionOffset?: PressRetentionOffset | undefined;
  /**
   * Indicates to accessibility services to treat UI component like a specific role.
   */
  role?: Role | undefined;
  /**
   * Lets the user select text for native copy and paste.
   *
   * @default `false`
   */
  selectable?: boolean | undefined;
  style?: TextStyleProp | undefined;
  testID?: string | undefined;
}>;
export interface TextProps extends Readonly<Omit<TextPointerEventProps, keyof TextPropsIOS | keyof TextPropsAndroid | keyof TextBaseProps | keyof AccessibilityProps | keyof {}> & Omit<TextPropsIOS, keyof TextPropsAndroid | keyof TextBaseProps | keyof AccessibilityProps | keyof {}> & Omit<TextPropsAndroid, keyof TextBaseProps | keyof AccessibilityProps | keyof {}> & Omit<TextBaseProps, keyof AccessibilityProps | keyof {}> & Omit<AccessibilityProps, keyof {}>> {}
