/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<30ed4bd72a54bb353386abada2cb18e2>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\TextInput\WindowsTextInputNativeComponent.js
 */

import type { HostComponent } from "../../../src/private/types/HostComponent";
import type { PartialViewConfig } from "../../Renderer/shims/ReactNativeTypes";
import type { ColorValue, TextStyleProp } from "../../StyleSheet/StyleSheet";
import type { BubblingEventHandler, DirectEventHandler, Double, Float, Int32, WithDefault } from "../../Types/CodegenTypes";
import type { ViewProps } from "../View/ViewPropTypes";
import type { TextInputNativeCommands } from "./TextInputNativeCommands";
export type KeyboardType = "default" | "email-address" | "numeric" | "phone-pad" | "number-pad" | "decimal-pad" | "url" | "ascii-capable" | "numbers-and-punctuation" | "name-phone-pad" | "twitter" | "web-search" | "visible-password";
export type ReturnKeyType = "done" | "go" | "next" | "search" | "send" | "none" | "previous" | "default" | "emergency-call" | "google" | "join" | "route" | "yahoo";
export type SubmitBehavior = "submit" | "blurAndSubmit" | "newline";
export type WindowsTextInputNativeProps = Readonly<Omit<Omit<ViewProps, "style">, "autoComplete" | "returnKeyLabel" | "numberOfLines" | "disableFullscreenUI" | "textBreakStrategy" | "underlineColorAndroid" | "inlineImageLeft" | "inlineImagePadding" | "importantForAutofill" | "showSoftInputOnFocus" | "autoCapitalize" | "autoCorrect" | "autoFocus" | "allowFontScaling" | "maxFontSizeMultiplier" | "editable" | "keyboardType" | "returnKeyType" | "maxLength" | "multiline" | "onBlur" | "onFocus" | "onChange" | "onChangeText" | "onContentSizeChange" | "onEndEditing" | "onSelectionChange" | "onSubmitEditing" | "onKeyPress" | "onScroll" | "placeholder" | "placeholderTextColor" | "secureTextEntry" | "selectionColor" | "selectionHandleColor" | "selection" | "value" | "defaultValue" | "selectTextOnFocus" | "submitBehavior" | "style" | "caretHidden" | "contextMenuHidden" | "textShadowColor" | "textShadowRadius" | "textDecorationLine" | "fontStyle" | "textShadowOffset" | "lineHeight" | "textTransform" | "color" | "letterSpacing" | "fontSize" | "textAlign" | "includeFontPadding" | "fontWeight" | "fontFamily" | "fontVariationSettings" | "textAlignVertical" | "cursorColor" | "mostRecentEventCount" | "text" | "clearTextOnFocus" | "clearTextOnSubmit" | "scrollEnabled" | "spellCheck"> & {
  /**
   * Android props after this
   */
  /**
   * Specifies autocomplete hints for the system, so it can provide autofill. On Android, the system will always attempt to offer autofill by using heuristics to identify the type of content.
   * To disable autocomplete, set `autoComplete` to `off`.
   *
   * *Android Only*
   *
   * Possible values for `autoComplete` are:
   *
   * - `birthdate-day`
   * - `birthdate-full`
   * - `birthdate-month`
   * - `birthdate-year`
   * - `cc-csc`
   * - `cc-exp`
   * - `cc-exp-day`
   * - `cc-exp-month`
   * - `cc-exp-year`
   * - `cc-number`
   * - `2fa-app-otp`
   * - `email`
   * - `email-otp`
   * - `flight-confirmation-code`
   * - `flight-number`
   * - `gender`
   * - `gift-card-number`
   * - `gift-card-pin`
   * - `loyalty-account-number`
   * - `name`
   * - `name-family`
   * - `name-given`
   * - `name-middle`
   * - `name-middle-initial`
   * - `name-prefix`
   * - `name-suffix`
   * - `password`
   * - `password-new`
   * - `postal-address`
   * - `postal-address-country`
   * - `postal-address-dependent-locality`
   * - `postal-address-extended`
   * - `postal-address-extended-postal-code`
   * - `postal-address-locality`
   * - `postal-address-region`
   * - `postal-address-unit`
   * - `postal-code`
   * - `promo-code`
   * - `street-address`
   * - `sms-otp`
   * - `tel`
   * - `tel-country-code`
   * - `tel-national`
   * - `tel-device`
   * - `upi-vpa`
   * - `wifi-password`
   * - `username`
   * - `username-new`
   * - `off`
   *
   * @platform android
   */
  autoComplete?: WithDefault<"2fa-app-otp" | "birthdate-day" | "birthdate-full" | "birthdate-month" | "birthdate-year" | "cc-csc" | "cc-exp" | "cc-exp-day" | "cc-exp-month" | "cc-exp-year" | "cc-number" | "email" | "email-otp" | "flight-confirmation-code" | "flight-number" | "gender" | "gift-card-number" | "gift-card-pin" | "loyalty-account-number" | "name" | "name-family" | "name-given" | "name-middle" | "name-middle-initial" | "name-prefix" | "name-suffix" | "password" | "password-new" | "postal-address" | "postal-address-country" | "postal-address-dependent-locality" | "postal-address-extended" | "postal-address-extended-postal-code" | "postal-address-locality" | "postal-address-region" | "postal-address-unit" | "postal-code" | "promo-code" | "street-address" | "sms-otp" | "tel" | "tel-country-code" | "tel-national" | "tel-device" | "upi-vpa" | "wifi-password" | "username" | "username-new" | "off", "off"> | undefined;
  /**
   * Sets the return key to the label. Use it instead of `returnKeyType`.
   * @platform android
   */
  returnKeyLabel?: string | undefined;
  /**
   * Sets the number of lines for a `TextInput`. Use it with multiline set to
   * `true` to be able to fill the lines.
   * @platform android
   */
  numberOfLines?: Int32 | undefined;
  /**
   * When `false`, if there is a small amount of space available around a text input
   * (e.g. landscape orientation on a phone), the OS may choose to have the user edit
   * the text inside of a full screen text input mode. When `true`, this feature is
   * disabled and users will always edit the text directly inside of the text input.
   * Defaults to `false`.
   * @platform android
   */
  disableFullscreenUI?: boolean | undefined;
  /**
   * Set text break strategy on Android API Level 23+, possible values are `simple`, `highQuality`, `balanced`
   * The default value is `simple`.
   * @platform android
   */
  textBreakStrategy?: WithDefault<"simple" | "highQuality" | "balanced", "simple"> | undefined;
  /**
   * The color of the `TextInput` underline.
   * @platform android
   */
  underlineColorAndroid?: ColorValue | undefined;
  /**
   * If defined, the provided image resource will be rendered on the left.
   * The image resource must be inside `/android/app/src/main/res/drawable` and referenced
   * like
   * ```
   * <TextInput
   *  inlineImageLeft='search_icon'
   * />
   * ```
   * @platform android
   */
  inlineImageLeft?: string | undefined;
  /**
   * Padding between the inline image, if any, and the text input itself.
   * @platform android
   */
  inlineImagePadding?: Int32 | undefined;
  importantForAutofill?: string | undefined;
  /**
   * When `false`, it will prevent the soft keyboard from showing when the field is focused.
   * Defaults to `true`.
   */
  showSoftInputOnFocus?: boolean | undefined;
  /**
   * TextInput props after this
   */
  /**
   * Can tell `TextInput` to automatically capitalize certain characters.
   *
   * - `characters`: all characters.
   * - `words`: first letter of each word.
   * - `sentences`: first letter of each sentence (*default*).
   * - `none`: don't auto capitalize anything.
   */
  autoCapitalize?: WithDefault<"none" | "sentences" | "words" | "characters", "none"> | undefined;
  /**
   * If `false`, disables auto-correct. The default value is `true`.
   */
  autoCorrect?: boolean | undefined;
  /**
   * If `true`, focuses the input on `componentDidMount`.
   * The default value is `false`.
   */
  autoFocus?: boolean | undefined;
  /**
   * Specifies whether fonts should scale to respect Text Size accessibility settings. The
   * default is `true`.
   */
  allowFontScaling?: boolean | undefined;
  /**
   * Specifies largest possible scale a font can reach when `allowFontScaling` is enabled.
   * Possible values:
   * `null/undefined` (default): inherit from the parent node or the global default (0)
   * `0`: no max, ignore parent/global default
   * `>= 1`: sets the maxFontSizeMultiplier of this node to this value
   */
  maxFontSizeMultiplier?: Float | undefined;
  /**
   * If `false`, text is not editable. The default value is `true`.
   */
  editable?: boolean | undefined;
  /**
   * Determines which keyboard to open, e.g.`numeric`.
   *
   * The following values work across platforms:
   *
   * - `default`
   * - `numeric`
   * - `number-pad`
   * - `decimal-pad`
   * - `email-address`
   * - `phone-pad`
   * - `url`
   *
   * *Android Only*
   *
   * The following values work on Android only:
   *
   * - `visible-password`
   */
  keyboardType?: WithDefault<KeyboardType, "default"> | undefined;
  /**
   * Determines how the return key should look. On Android you can also use
   * `returnKeyLabel`.
   *
   * *Cross platform*
   *
   * The following values work across platforms:
   *
   * - `done`
   * - `go`
   * - `next`
   * - `search`
   * - `send`
   *
   * *Android Only*
   *
   * The following values work on Android only:
   *
   * - `none`
   * - `previous`
   */
  returnKeyType?: WithDefault<ReturnKeyType, "done"> | undefined;
  /**
   * Limits the maximum number of characters that can be entered. Use this
   * instead of implementing the logic in JS to avoid flicker.
   */
  maxLength?: Int32 | undefined;
  /**
   * If `true`, the text input can be multiple lines.
   * The default value is `false`.
   */
  multiline?: boolean | undefined;
  /**
   * Callback that is called when the text input is blurred.
   * `target` is the reactTag of the element
   */
  onBlur?: BubblingEventHandler<Readonly<{
    target: Int32;
  }>> | undefined;
  /**
   * Callback that is called when the text input is focused.
   * `target` is the reactTag of the element
   */
  onFocus?: BubblingEventHandler<Readonly<{
    target: Int32;
  }>> | undefined;
  /**
   * Callback that is called when the text input's text changes.
   * `target` is the reactTag of the element
   * TODO: differentiate between onChange and onChangeText
   */
  onChange?: BubblingEventHandler<Readonly<{
    target: Int32;
    eventCount: Int32;
    text: string;
  }>> | undefined;
  /**
   * Callback that is called when the text input's text changes.
   * Changed text is passed as an argument to the callback handler.
   * TODO: differentiate between onChange and onChangeText
   */
  onChangeText?: BubblingEventHandler<Readonly<{
    target: Int32;
    eventCount: Int32;
    text: string;
  }>> | undefined;
  /**
   * Callback that is called when the text input's content size changes.
   * This will be called with
   * `{ nativeEvent: { contentSize: { width, height } } }`.
   *
   * Only called for multiline text inputs.
   */
  onContentSizeChange?: DirectEventHandler<Readonly<{
    target: Int32;
    contentSize: Readonly<{
      width: Double;
      height: Double;
    }>;
  }>> | undefined;
  /**
   * Callback that is called when text input ends.
   */
  onEndEditing?: BubblingEventHandler<Readonly<{
    target: Int32;
    text: string;
  }>> | undefined;
  /**
   * Callback that is called when the text input selection is changed.
   * This will be called with
   * `{ nativeEvent: { selection: { start, end } } }`.
   */
  onSelectionChange?: DirectEventHandler<Readonly<{
    target: Int32;
    selection: Readonly<{
      start: Double;
      end: Double;
    }>;
  }>> | undefined;
  /**
   * Callback that is called when the text input's submit button is pressed.
   * Invalid if `multiline={true}` is specified.
   */
  onSubmitEditing?: BubblingEventHandler<Readonly<{
    target: Int32;
    text: string;
  }>> | undefined;
  /**
   * Callback that is called when a key is pressed.
   * This will be called with `{ nativeEvent: { key: keyValue } }`
   * where `keyValue` is `'Enter'` or `'Backspace'` for respective keys and
   * the typed-in character otherwise including `' '` for space.
   * Fires before `onChange` callbacks.
   */
  onKeyPress?: BubblingEventHandler<Readonly<{
    target: Int32;
    key: string;
  }>> | undefined;
  /**
   * Invoked on content scroll with `{ nativeEvent: { contentOffset: { x, y } } }`.
   * May also contain other properties from ScrollEvent but on Android contentSize
   * is not provided for performance reasons.
   */
  onScroll?: DirectEventHandler<Readonly<{
    target: Int32;
    responderIgnoreScroll: boolean;
    contentInset: Readonly<{
      top: Double;
      bottom: Double;
      left: Double;
      right: Double;
    }>;
    contentOffset: Readonly<{
      x: Double;
      y: Double;
    }>;
    contentSize: Readonly<{
      width: Double;
      height: Double;
    }>;
    layoutMeasurement: Readonly<{
      width: Double;
      height: Double;
    }>;
    velocity: Readonly<{
      x: Double;
      y: Double;
    }>;
  }>> | undefined;
  /**
   * The string that will be rendered before text input has been entered.
   */
  placeholder?: string | undefined;
  /**
   * The text color of the placeholder string.
   */
  placeholderTextColor?: ColorValue | undefined;
  /**
   * If `true`, the text input obscures the text entered so that sensitive text
   * like passwords stay secure. The default value is `false`. Does not work with 'multiline={true}'.
   */
  secureTextEntry?: boolean | undefined;
  /**
   * The highlight and cursor color of the text input.
   */
  selectionColor?: ColorValue | undefined;
  /**
   * The text selection handle color.
   */
  selectionHandleColor?: ColorValue | undefined;
  /**
   * The start and end of the text input's selection. Set start and end to
   * the same value to position the cursor.
   */
  selection?: Readonly<{
    start: Int32;
    end?: Int32 | undefined;
  }> | undefined;
  /**
   * The value to show for the text input. `TextInput` is a controlled
   * component, which means the native value will be forced to match this
   * value prop if provided. For most uses, this works great, but in some
   * cases this may cause flickering - one common cause is preventing edits
   * by keeping value the same. In addition to simply setting the same value,
   * either set `editable={false}`, or set/update `maxLength` to prevent
   * unwanted edits without flicker.
   */
  value?: string | undefined;
  /**
   * Provides an initial value that will change when the user starts typing.
   * Useful for simple use-cases where you do not want to deal with listening
   * to events and updating the value prop to keep the controlled state in sync.
   */
  defaultValue?: string | undefined;
  /**
   * If `true`, all text will automatically be selected on focus.
   */
  selectTextOnFocus?: boolean | undefined;
  /**
   * When the return key is pressed,
   *
   * For single line inputs:
   *
   * - `'newline`' defaults to `'blurAndSubmit'`
   * - `undefined` defaults to `'blurAndSubmit'`
   *
   * For multiline inputs:
   *
   * - `'newline'` adds a newline
   * - `undefined` defaults to `'newline'`
   *
   * For both single line and multiline inputs:
   *
   * - `'submit'` will only send a submit event and not blur the input
   * - `'blurAndSubmit`' will both blur the input and send a submit event
   */
  submitBehavior?: SubmitBehavior | undefined;
  /**
   * Note that not all Text styles are supported, an incomplete list of what is not supported includes:
   *
   * - `borderLeftWidth`
   * - `borderTopWidth`
   * - `borderRightWidth`
   * - `borderBottomWidth`
   * - `borderTopLeftRadius`
   * - `borderTopRightRadius`
   * - `borderBottomRightRadius`
   * - `borderBottomLeftRadius`
   *
   * see [Issue#7070](https://github.com/facebook/react-native/issues/7070)
   * for more detail.
   *
   * [Styles](docs/style.html)
   */
  style?: TextStyleProp | undefined;
  /**
   * If `true`, caret is hidden. The default value is `false`.
   * This property is supported only for single-line TextInput component on iOS.
   */
  caretHidden?: boolean | undefined;
  contextMenuHidden?: boolean | undefined;
  /**
   * The following are props that `BaseTextShadowNode` takes. It is unclear if they
   * are used by TextInput.
   */
  textShadowColor?: ColorValue | undefined;
  textShadowRadius?: Float | undefined;
  textDecorationLine?: string | undefined;
  fontStyle?: string | undefined;
  textShadowOffset?: Readonly<{
    width?: Double | undefined;
    height?: Double | undefined;
  }> | undefined;
  lineHeight?: Float | undefined;
  textTransform?: string | undefined;
  color?: Int32 | undefined;
  letterSpacing?: Float | undefined;
  fontSize?: Float | undefined;
  textAlign?: string | undefined;
  includeFontPadding?: boolean | undefined;
  fontWeight?: string | undefined;
  fontFamily?: string | undefined;
  fontVariationSettings?: string | undefined;
  /**
   * I cannot find where these are defined but JS complains without them.
   */
  textAlignVertical?: string | undefined;
  cursorColor?: ColorValue | undefined;
  /**
   * "Private" fields used by TextInput.js and not users of this component directly
   */
  mostRecentEventCount: Int32;
  text?: string | undefined;
  clearTextOnFocus?: boolean | undefined;
  clearTextOnSubmit?: boolean | undefined;
  scrollEnabled?: boolean | undefined;
  spellCheck?: boolean | undefined;
}>;
type NativeType = HostComponent<WindowsTextInputNativeProps>;
type NativeCommands = TextInputNativeCommands<NativeType>;
export declare const Commands: NativeCommands;
export declare type Commands = typeof Commands;
export declare const __INTERNAL_VIEW_CONFIG: PartialViewConfig;
export declare type __INTERNAL_VIEW_CONFIG = typeof __INTERNAL_VIEW_CONFIG;
declare const $$WindowsTextInputNativeComponent: HostComponent<WindowsTextInputNativeProps>;
declare type $$WindowsTextInputNativeComponent = typeof $$WindowsTextInputNativeComponent;
export default $$WindowsTextInputNativeComponent;
