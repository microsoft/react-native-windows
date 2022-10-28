/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import type {
  LayoutEvent,
  PointerEvent,
  PressEvent,
  TextLayoutEvent,
  // [Windows
  BlurEvent,
  FocusEvent,
  KeyEvent, // Windows]
} from '../Types/CoreEventTypes';
import type {Node} from 'react';
import type {TextStyleProp} from '../StyleSheet/StyleSheet';
import type {
  AccessibilityRole,
  AccessibilityState,
  AccessibilityActionInfo,
  AccessibilityActionEvent,
} from '../Components/View/ViewAccessibility';

export type PressRetentionOffset = $ReadOnly<{|
  top: number,
  left: number,
  bottom: number,
  right: number,
|}>;

type PointerEventProps = $ReadOnly<{|
  onPointerEnter?: (event: PointerEvent) => void,
  onPointerLeave?: (event: PointerEvent) => void,
  onPointerMove?: (event: PointerEvent) => void,
|}>;

type WindowsTextProps = $ReadOnly<{|
  /**
   * Key up event
   *
   * @platform windows
   */
  onKeyUp?: ?(e: KeyEvent) => void,
  onKeyUpCapture?: ?(e: KeyEvent) => void,
  keyUpEvents?: ?$ReadOnlyArray<HandledKeyboardEvent>,

  onKeyDown?: ?(e: KeyEvent) => void,
  onKeyDownCapture?: ?(e: KeyEvent) => void,
  keyDownEvents?: ?$ReadOnlyArray<HandledKeyboardEvent>,

  /**
   * Specifies if the control should show System focus visuals
   */
  enableFocusRing?: ?boolean,

  onFocus?: ?(event: FocusEvent) => mixed,
  onBlur?: ?(event: FocusEvent) => mixed,
  onHoverIn?: ?(event: MouseEvent) => mixed,
  onHoverOut?: ?(event: MouseEvent) => mixed,
  /**
  * Duration to wait after hover in before calling `onHoverIn`.
  */
  delayHoverIn?: ?number,
  /**
  * Duration to wait after hover out before calling `onHoverOut`.
  */
  delayHoverOut?: ?number,
|}>;
// Windows]

/**
 * @see https://reactnative.dev/docs/text#reference
 */
export type TextProps = $ReadOnly<{|
  ...PointerEventProps,
  ...WindowsViewProps, // [Windows]

  /**
   * Indicates whether the view is an accessibility element.
   *
   * See https://reactnative.dev/docs/text#accessible
   */
  accessible?: ?boolean,
  accessibilityActions?: ?$ReadOnlyArray<AccessibilityActionInfo>,
  onAccessibilityAction?: ?(event: AccessibilityActionEvent) => mixed,
  accessibilityHint?: ?Stringish,
  accessibilityLanguage?: ?Stringish,
  accessibilityLabel?: ?Stringish,
  accessibilityRole?: ?AccessibilityRole,
  accessibilityState?: ?AccessibilityState,
  'aria-label'?: ?string,

  /**
   * Whether font should be scaled down automatically.
   *
   * See https://reactnative.dev/docs/text#adjustsfontsizetofit
   */
  adjustsFontSizeToFit?: ?boolean,

  /**
   * Whether fonts should scale to respect Text Size accessibility settings.
   *
   * See https://reactnative.dev/docs/text#allowfontscaling
   */
  allowFontScaling?: ?boolean,

  /**
   * Set hyphenation strategy on Android.
   *
   */
  android_hyphenationFrequency?: ?('normal' | 'none' | 'full'),

  /**
   * alias for accessibilityState
   *
   * see https://reactnative.dev/docs/accessibility#accessibilitystate
   */
  'aria-busy'?: ?boolean,
  'aria-checked'?: ?boolean,
  'aria-disabled'?: ?boolean,
  'aria-expanded'?: ?boolean,
  'aria-selected'?: ?boolean,
  children?: ?Node,

  /**
   * When `numberOfLines` is set, this prop defines how text will be
   * truncated.
   *
   * See https://reactnative.dev/docs/text#ellipsizemode
   */
  ellipsizeMode?: ?('clip' | 'head' | 'middle' | 'tail'),

  /**
   * Used to locate this view from native code.
   *
   * See https://reactnative.dev/docs/text#nativeid
   */
  id?: string,

  /**
   * Specifies largest possible scale a font can reach when `allowFontScaling` is enabled.
   * Possible values:
   * `null/undefined` (default): inherit from the parent node or the global default (0)
   * `0`: no max, ignore parent/global default
   * `>= 1`: sets the maxFontSizeMultiplier of this node to this value
   */
  maxFontSizeMultiplier?: ?number,

  /**
   * Used to locate this view from native code.
   *
   * See https://reactnative.dev/docs/text#nativeid
   */
  nativeID?: ?string,

  /**
   * Used to truncate the text with an ellipsis.
   *
   * See https://reactnative.dev/docs/text#numberoflines
   */
  numberOfLines?: ?number,

  /**
   * Invoked on mount and layout changes.
   *
   * See https://reactnative.dev/docs/text#onlayout
   */
  onLayout?: ?(event: LayoutEvent) => mixed,

  /**
   * This function is called on long press.
   *
   * See https://reactnative.dev/docs/text#onlongpress
   */
  onLongPress?: ?(event: PressEvent) => mixed,

  /**
   * This function is called on press.
   *
   * See https://reactnative.dev/docs/text#onpress
   */
  onPress?: ?(event: PressEvent) => mixed,
  onPressIn?: ?(event: PressEvent) => mixed,
  onPressOut?: ?(event: PressEvent) => mixed,
  onResponderGrant?: ?(event: PressEvent) => void,
  onResponderMove?: ?(event: PressEvent) => void,
  onResponderRelease?: ?(event: PressEvent) => void,
  onResponderTerminate?: ?(event: PressEvent) => void,
  onResponderTerminationRequest?: ?() => boolean,
  onStartShouldSetResponder?: ?() => boolean,
  onMoveShouldSetResponder?: ?() => boolean,
  onTextLayout?: ?(event: TextLayoutEvent) => mixed,

  /**
   * Defines how far your touch may move off of the button, before
   * deactivating the button.
   *
   * See https://reactnative.dev/docs/text#pressretentionoffset
   */
  pressRetentionOffset?: ?PressRetentionOffset,

  /**
   * Lets the user select text.
   *
   * See https://reactnative.dev/docs/text#selectable
   */
  selectable?: ?boolean,
  style?: ?TextStyleProp,

  /**
   * Used to locate this view in end-to-end tests.
   *
   * See https://reactnative.dev/docs/text#testid
   */
  testID?: ?string,

  /**
   * Android Only
   */

  /**
   * Specifies the disabled state of the text view for testing purposes.
   *
   * See https://reactnative.dev/docs/text#disabled
   */
  disabled?: ?boolean,

  /**
   * The highlight color of the text.
   *
   * See https://reactnative.dev/docs/text#selectioncolor
   */
  selectionColor?: ?string,

  dataDetectorType?: ?('phoneNumber' | 'link' | 'email' | 'none' | 'all'),

  /**
   * Set text break strategy on Android.
   *
   * See https://reactnative.dev/docs/text#textbreakstrategy
   */
  textBreakStrategy?: ?('balanced' | 'highQuality' | 'simple'),

  /**
   * iOS Only
   */
  adjustsFontSizeToFit?: ?boolean,

  /**
   * Smallest possible scale a font can reach.
   *
   * See https://reactnative.dev/docs/text#minimumfontscale
   */
  minimumFontScale?: ?number,

  /**
   * When `true`, no visual change is made when text is pressed down.
   *
   * See https://reactnative.dev/docs/text#supperhighlighting
   */
  suppressHighlighting?: ?boolean,
|}>;
