/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import * as React from 'react';

import DeprecatedTextInputPropTypes from '../../DeprecatedPropTypes/DeprecatedTextInputPropTypes';

import Platform from '../../Utilities/Platform';
import StyleSheet, {
  type TextStyleProp,
  type ViewStyleProp,
  type ColorValue,
} from '../../StyleSheet/StyleSheet';
import Text from '../../Text/Text';
import TextAncestor from '../../Text/TextAncestor';
import TextInputState from './TextInputState';
import invariant from 'invariant';
import nullthrows from 'nullthrows';
import setAndForwardRef from '../../Utilities/setAndForwardRef';

import usePressability from '../../Pressability/usePressability';

import type {ViewProps} from '../View/ViewPropTypes';
import type {
  SyntheticEvent,
  ScrollEvent,
  PressEvent,
} from '../../Types/CoreEventTypes';
import type {HostComponent} from '../../Renderer/shims/ReactNativeTypes';
import type {TextInputNativeCommands} from './TextInputNativeCommands';

const {useLayoutEffect, useRef, useState} = React;

type ReactRefSetter<T> = {current: null | T, ...} | ((ref: null | T) => mixed);

let AndroidTextInput;
let AndroidTextInputCommands;
let RCTSinglelineTextInputView;
let RCTSinglelineTextInputNativeCommands;
let RCTMultilineTextInputView;
let RCTMultilineTextInputNativeCommands;
let WindowsTextInput; // [Windows]
let WindowsTextInputCommands; // [Windows]
import type {KeyEvent} from '../../Types/CoreEventTypes'; // [Windows]

// [Windows
if (Platform.OS === 'android') {
  AndroidTextInput = require('./AndroidTextInputNativeComponent').default;
  AndroidTextInputCommands = require('./AndroidTextInputNativeComponent')
    .Commands;
} else if (Platform.OS === 'ios') {
  RCTSinglelineTextInputView = require('./RCTSingelineTextInputNativeComponent')
    .default;
  RCTSinglelineTextInputNativeCommands = require('./RCTSingelineTextInputNativeComponent')
    .Commands;
  RCTMultilineTextInputView = require('./RCTMultilineTextInputNativeComponent')
    .default;
  RCTMultilineTextInputNativeCommands = require('./RCTMultilineTextInputNativeComponent')
    .Commands;
}
// [Windows
else if (Platform.OS === 'windows') {
  WindowsTextInput = require('./WindowsTextInputNativeComponent').default;
  WindowsTextInputCommands = require('./WindowsTextInputNativeComponent')
    .Commands;
}
// Windows]

export type ChangeEvent = SyntheticEvent<
  $ReadOnly<{|
    eventCount: number,
    target: number,
    text: string,
  |}>,
>;

export type TextInputEvent = SyntheticEvent<
  $ReadOnly<{|
    eventCount: number,
    previousText: string,
    range: $ReadOnly<{|
      start: number,
      end: number,
    |}>,
    target: number,
    text: string,
  |}>,
>;

export type ContentSizeChangeEvent = SyntheticEvent<
  $ReadOnly<{|
    target: number,
    contentSize: $ReadOnly<{|
      width: number,
      height: number,
    |}>,
  |}>,
>;

type TargetEvent = SyntheticEvent<
  $ReadOnly<{|
    target: number,
  |}>,
>;

export type BlurEvent = TargetEvent;
export type FocusEvent = TargetEvent;

type Selection = $ReadOnly<{|
  start: number,
  end: number,
|}>;

export type SelectionChangeEvent = SyntheticEvent<
  $ReadOnly<{|
    selection: Selection,
    target: number,
  |}>,
>;

export type KeyPressEvent = SyntheticEvent<
  $ReadOnly<{|
    key: string,
    target?: ?number,
    eventCount?: ?number,
  |}>,
>;

export type EditingEvent = SyntheticEvent<
  $ReadOnly<{|
    eventCount: number,
    text: string,
    target: number,
  |}>,
>;

type DataDetectorTypesType =
  | 'phoneNumber'
  | 'link'
  | 'address'
  | 'calendarEvent'
  | 'none'
  | 'all';

export type KeyboardType =
  // Cross Platform
  | 'default'
  | 'email-address'
  | 'numeric'
  | 'phone-pad'
  | 'number-pad'
  | 'decimal-pad'
  | 'url'
  // iOS-only
  | 'ascii-capable'
  | 'numbers-and-punctuation'
  | 'name-phone-pad'
  | 'twitter'
  | 'web-search'
  // iOS 10+ only
  | 'ascii-capable-number-pad'
  // Android-only
  | 'visible-password';

export type ReturnKeyType =
  // Cross Platform
  | 'done'
  | 'go'
  | 'next'
  | 'search'
  | 'send'
  // Android-only
  | 'none'
  | 'previous'
  // iOS-only
  | 'default'
  | 'emergency-call'
  | 'google'
  | 'join'
  | 'route'
  | 'yahoo';

export type AutoCapitalize = 'none' | 'sentences' | 'words' | 'characters';

export type TextContentType =
  | 'none'
  | 'URL'
  | 'addressCity'
  | 'addressCityAndState'
  | 'addressState'
  | 'countryName'
  | 'creditCardNumber'
  | 'emailAddress'
  | 'familyName'
  | 'fullStreetAddress'
  | 'givenName'
  | 'jobTitle'
  | 'location'
  | 'middleName'
  | 'name'
  | 'namePrefix'
  | 'nameSuffix'
  | 'nickname'
  | 'organizationName'
  | 'postalCode'
  | 'streetAddressLine1'
  | 'streetAddressLine2'
  | 'sublocality'
  | 'telephoneNumber'
  | 'username'
  | 'password'
  | 'newPassword'
  | 'oneTimeCode';

type PasswordRules = string;

type IOSProps = $ReadOnly<{|
  /**
   * When the clear button should appear on the right side of the text view.
   * This property is supported only for single-line TextInput component.
   * @platform ios
   */
  clearButtonMode?: ?('never' | 'while-editing' | 'unless-editing' | 'always'),

  /**
   * If `true`, clears the text field automatically when editing begins.
   * @platform ios
   */
  clearTextOnFocus?: ?boolean,

  /**
   * Determines the types of data converted to clickable URLs in the text input.
   * Only valid if `multiline={true}` and `editable={false}`.
   * By default no data types are detected.
   *
   * You can provide one type or an array of many types.
   *
   * Possible values for `dataDetectorTypes` are:
   *
   * - `'phoneNumber'`
   * - `'link'`
   * - `'address'`
   * - `'calendarEvent'`
   * - `'none'`
   * - `'all'`
   *
   * @platform ios
   */
  dataDetectorTypes?:
    | ?DataDetectorTypesType
    | $ReadOnlyArray<DataDetectorTypesType>,

  /**
   * If `true`, the keyboard disables the return key when there is no text and
   * automatically enables it when there is text. The default value is `false`.
   * @platform ios
   */
  enablesReturnKeyAutomatically?: ?boolean,

  /**
   * An optional identifier which links a custom InputAccessoryView to
   * this text input. The InputAccessoryView is rendered above the
   * keyboard when this text input is focused.
   * @platform ios
   */
  inputAccessoryViewID?: ?string,

  /**
   * Determines the color of the keyboard.
   * @platform ios
   */
  keyboardAppearance?: ?('default' | 'light' | 'dark'),

  /**
   * Provide rules for your password.
   * For example, say you want to require a password with at least eight characters consisting of a mix of uppercase and lowercase letters, at least one number, and at most two consecutive characters.
   * "required: upper; required: lower; required: digit; max-consecutive: 2; minlength: 8;"
   * @platform ios
   */
  passwordRules?: ?PasswordRules,

  /*
   * If `true`, allows TextInput to pass touch events to the parent component.
   * This allows components to be swipeable from the TextInput on iOS,
   * as is the case on Android by default.
   * If `false`, TextInput always asks to handle the input (except when disabled).
   * @platform ios
   */
  rejectResponderTermination?: ?boolean,

  /**
   * If `false`, scrolling of the text view will be disabled.
   * The default value is `true`. Does only work with 'multiline={true}'.
   * @platform ios
   */
  scrollEnabled?: ?boolean,

  /**
   * If `false`, disables spell-check style (i.e. red underlines).
   * The default value is inherited from `autoCorrect`.
   * @platform ios
   */
  spellCheck?: ?boolean,

  /**
   * Give the keyboard and the system information about the
   * expected semantic meaning for the content that users enter.
   * @platform ios
   */
  textContentType?: ?TextContentType,
|}>;

type AndroidProps = $ReadOnly<{|
  /**
   * Determines which content to suggest on auto complete, e.g.`username`.
   * To disable auto complete, use `off`.
   *
   * *Android Only*
   *
   * The following values work on Android only:
   *
   * - `username`
   * - `password`
   * - `email`
   * - `name`
   * - `tel`
   * - `street-address`
   * - `postal-code`
   * - `cc-number`
   * - `cc-csc`
   * - `cc-exp`
   * - `cc-exp-month`
   * - `cc-exp-year`
   * - `off`
   *
   * @platform android
   */
  autoComplete?: ?(
    | 'birthdate-day'
    | 'birthdate-full'
    | 'birthdate-month'
    | 'birthdate-year'
    | 'cc-csc'
    | 'cc-exp'
    | 'cc-exp-day'
    | 'cc-exp-month'
    | 'cc-exp-year'
    | 'cc-number'
    | 'email'
    | 'gender'
    | 'name'
    | 'name-family'
    | 'name-given'
    | 'name-middle'
    | 'name-middle-initial'
    | 'name-prefix'
    | 'name-suffix'
    | 'password'
    | 'password-new'
    | 'postal-address'
    | 'postal-address-country'
    | 'postal-address-extended'
    | 'postal-address-extended-postal-code'
    | 'postal-address-locality'
    | 'postal-address-region'
    | 'postal-code'
    | 'street-address'
    | 'sms-otp'
    | 'tel'
    | 'tel-country-code'
    | 'tel-national'
    | 'tel-device'
    | 'username'
    | 'username-new'
    | 'off'
  ),

  /**
   * When `false`, if there is a small amount of space available around a text input
   * (e.g. landscape orientation on a phone), the OS may choose to have the user edit
   * the text inside of a full screen text input mode. When `true`, this feature is
   * disabled and users will always edit the text directly inside of the text input.
   * Defaults to `false`.
   * @platform android
   */
  disableFullscreenUI?: ?boolean,

  importantForAutofill?: ?(
    | 'auto'
    | 'no'
    | 'noExcludeDescendants'
    | 'yes'
    | 'yesExcludeDescendants'
  ),

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
  inlineImageLeft?: ?string,

  /**
   * Padding between the inline image, if any, and the text input itself.
   * @platform android
   */
  inlineImagePadding?: ?number,

  /**
   * Sets the number of lines for a `TextInput`. Use it with multiline set to
   * `true` to be able to fill the lines.
   * @platform android
   */
  numberOfLines?: ?number,

  /**
   * Sets the return key to the label. Use it instead of `returnKeyType`.
   * @platform android
   */
  returnKeyLabel?: ?string,

  /**
   * When `false`, it will prevent the soft keyboard from showing when the field is focused.
   * Defaults to `true`.
   */
  showSoftInputOnFocus?: ?boolean,

  /**
   * Set text break strategy on Android API Level 23+, possible values are `simple`, `highQuality`, `balanced`
   * The default value is `simple`.
   * @platform android
   */
  textBreakStrategy?: ?('simple' | 'highQuality' | 'balanced'),

  /**
   * The color of the `TextInput` underline.
   * @platform android
   */
  underlineColorAndroid?: ?ColorValue,
|}>;

// [Windows

type SubmitKeyEvent = $ReadOnly<{|
  altKey?: ?boolean,
  ctrlKey?: ?boolean,
  metaKey?: ?boolean,
  shiftKey?: ?boolean,
  code: string,
|}>;

type WindowsProps = $ReadOnly<{|
  /**
   * If `true`, clears the text field synchronously before `onSubmitEditing` is emitted.
   * @platform windows
   */
  clearTextOnSubmit?: ?boolean,

  /**
   * Configures keys that can be used to submit editing for the TextInput.
   * @platform windows
   */
  submitKeyEvents?: ?$ReadOnlyArray<SubmitKeyEvent>,
|}>;

// Windows]

export type Props = $ReadOnly<{|
  ...$Diff<ViewProps, $ReadOnly<{|style: ?ViewStyleProp|}>>,
  ...IOSProps,
  ...AndroidProps,
  ...WindowsProps, // [Windows]

  /**
   * Can tell `TextInput` to automatically capitalize certain characters.
   *
   * - `characters`: all characters.
   * - `words`: first letter of each word.
   * - `sentences`: first letter of each sentence (*default*).
   * - `none`: don't auto capitalize anything.
   */
  autoCapitalize?: ?AutoCapitalize,

  /**
   * If `false`, disables auto-correct. The default value is `true`.
   */
  autoCorrect?: ?boolean,

  /**
   * If `true`, focuses the input on `componentDidMount`.
   * The default value is `false`.
   */
  autoFocus?: ?boolean,

  /**
   * Specifies whether fonts should scale to respect Text Size accessibility settings. The
   * default is `true`.
   */
  allowFontScaling?: ?boolean,

  /**
   * If `true`, the text field will blur when submitted.
   * The default value is true for single-line fields and false for
   * multiline fields. Note that for multiline fields, setting `blurOnSubmit`
   * to `true` means that pressing return will blur the field and trigger the
   * `onSubmitEditing` event instead of inserting a newline into the field.
   */
  blurOnSubmit?: ?boolean,

  /**
   * If `true`, caret is hidden. The default value is `false`.
   *
   * On Android devices manufactured by Xiaomi with Android Q,
   * when keyboardType equals 'email-address'this will be set
   * in native to 'true' to prevent a system related crash. This
   * will cause cursor to be diabled as a side-effect.
   *
   */
  caretHidden?: ?boolean,

  /*
   * If `true`, contextMenuHidden is hidden. The default value is `false`.
   */
  contextMenuHidden?: ?boolean,

  /**
   * Provides an initial value that will change when the user starts typing.
   * Useful for simple use-cases where you do not want to deal with listening
   * to events and updating the value prop to keep the controlled state in sync.
   */
  defaultValue?: ?Stringish,

  /**
   * If `false`, text is not editable. The default value is `true`.
   */
  editable?: ?boolean,

  forwardedRef?: ?ReactRefSetter<
    React.ElementRef<HostComponent<mixed>> & ImperativeMethods,
  >,

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
   * *iOS Only*
   *
   * The following values work on iOS only:
   *
   * - `ascii-capable`
   * - `numbers-and-punctuation`
   * - `name-phone-pad`
   * - `twitter`
   * - `web-search`
   *
   * *Android Only*
   *
   * The following values work on Android only:
   *
   * - `visible-password`
   *
   */
  keyboardType?: ?KeyboardType,

  /**
   * Specifies largest possible scale a font can reach when `allowFontScaling` is enabled.
   * Possible values:
   * `null/undefined` (default): inherit from the parent node or the global default (0)
   * `0`: no max, ignore parent/global default
   * `>= 1`: sets the maxFontSizeMultiplier of this node to this value
   */
  maxFontSizeMultiplier?: ?number,

  /**
   * Limits the maximum number of characters that can be entered. Use this
   * instead of implementing the logic in JS to avoid flicker.
   */
  maxLength?: ?number,

  /**
   * If `true`, the text input can be multiple lines.
   * The default value is `false`.
   */
  multiline?: ?boolean,

  /**
   * Callback that is called when the text input is blurred.
   */
  onBlur?: ?(e: BlurEvent) => mixed,

  /**
   * Callback that is called when the text input's text changes.
   */
  onChange?: ?(e: ChangeEvent) => mixed,

  /**
   * Callback that is called when the text input's text changes.
   * Changed text is passed as an argument to the callback handler.
   */
  onChangeText?: ?(text: string) => mixed,

  /**
   * Callback that is called when the text input's content size changes.
   * This will be called with
   * `{ nativeEvent: { contentSize: { width, height } } }`.
   *
   * Only called for multiline text inputs.
   */
  onContentSizeChange?: ?(e: ContentSizeChangeEvent) => mixed,

  /**
   * Callback that is called when text input ends.
   */
  onEndEditing?: ?(e: EditingEvent) => mixed,

  /**
   * Callback that is called when the text input is focused.
   */
  onFocus?: ?(e: FocusEvent) => mixed,

  /**
   * Callback that is called when a key is pressed.
   * This will be called with `{ nativeEvent: { key: keyValue } }`
   * where `keyValue` is `'Enter'` or `'Backspace'` for respective keys and
   * the typed-in character otherwise including `' '` for space.
   * Fires before `onChange` callbacks.
   */
  onKeyPress?: ?(e: KeyPressEvent) => mixed,

  /**
   * Called when a touch is engaged.
   */
  onPressIn?: ?(event: PressEvent) => mixed,

  /**
   * Called when a touch is released.
   */
  onPressOut?: ?(event: PressEvent) => mixed,

  /**
   * Callback that is called when the text input selection is changed.
   * This will be called with
   * `{ nativeEvent: { selection: { start, end } } }`.
   */
  onSelectionChange?: ?(e: SelectionChangeEvent) => mixed,

  /**
   * Callback that is called when the text input's submit button is pressed.
   * Invalid if `multiline={true}` is specified.
   */
  onSubmitEditing?: ?(e: EditingEvent) => mixed,

  /**
   * Invoked on content scroll with `{ nativeEvent: { contentOffset: { x, y } } }`.
   * May also contain other properties from ScrollEvent but on Android contentSize
   * is not provided for performance reasons.
   */
  onScroll?: ?(e: ScrollEvent) => mixed,

  /**
   * The string that will be rendered before text input has been entered.
   */
  placeholder?: ?Stringish,

  /**
   * The text color of the placeholder string.
   */
  placeholderTextColor?: ?ColorValue,

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
   *
   * *iOS Only*
   *
   * The following values work on iOS only:
   *
   * - `default`
   * - `emergency-call`
   * - `google`
   * - `join`
   * - `route`
   * - `yahoo`
   */
  returnKeyType?: ?ReturnKeyType,

  /**
   * If `true`, the text input obscures the text entered so that sensitive text
   * like passwords stay secure. The default value is `false`. Does not work with 'multiline={true}'.
   */
  secureTextEntry?: ?boolean,

  /**
   * The start and end of the text input's selection. Set start and end to
   * the same value to position the cursor.
   */
  selection?: ?$ReadOnly<{|
    start: number,
    end?: ?number,
  |}>,

  /**
   * The highlight and cursor color of the text input.
   */
  selectionColor?: ?ColorValue,

  /**
   * If `true`, all text will automatically be selected on focus.
   */
  selectTextOnFocus?: ?boolean,

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
  style?: ?TextStyleProp,

  /**
   * The value to show for the text input. `TextInput` is a controlled
   * component, which means the native value will be forced to match this
   * value prop if provided. For most uses, this works great, but in some
   * cases this may cause flickering - one common cause is preventing edits
   * by keeping value the same. In addition to simply setting the same value,
   * either set `editable={false}`, or set/update `maxLength` to prevent
   * unwanted edits without flicker.
   */
  value?: ?Stringish,
|}>;

type ImperativeMethods = $ReadOnly<{|
  clear: () => void,
  isFocused: () => boolean,
  getNativeRef: () => ?React.ElementRef<HostComponent<mixed>>,
|}>;

const emptyFunctionThatReturnsTrue = () => true;

/**
 * A foundational component for inputting text into the app via a
 * keyboard. Props provide configurability for several features, such as
 * auto-correction, auto-capitalization, placeholder text, and different keyboard
 * types, such as a numeric keypad.
 *
 * The simplest use case is to plop down a `TextInput` and subscribe to the
 * `onChangeText` events to read the user input. There are also other events,
 * such as `onSubmitEditing` and `onFocus` that can be subscribed to. A simple
 * example:
 *
 * ```ReactNativeWebPlayer
 * import React, { Component } from 'react';
 * import { AppRegistry, TextInput } from 'react-native';
 *
 * export default class UselessTextInput extends Component {
 *   constructor(props) {
 *     super(props);
 *     this.state = { text: 'Useless Placeholder' };
 *   }
 *
 *   render() {
 *     return (
 *       <TextInput
 *         style={{height: 40, borderColor: 'gray', borderWidth: 1}}
 *         onChangeText={(text) => this.setState({text})}
 *         value={this.state.text}
 *       />
 *     );
 *   }
 * }
 *
 * // skip this line if using Create React Native App
 * AppRegistry.registerComponent('AwesomeProject', () => UselessTextInput);
 * ```
 *
 * Two methods exposed via the native element are .focus() and .blur() that
 * will focus or blur the TextInput programmatically.
 *
 * Note that some props are only available with `multiline={true/false}`.
 * Additionally, border styles that apply to only one side of the element
 * (e.g., `borderBottomColor`, `borderLeftWidth`, etc.) will not be applied if
 * `multiline=false`. To achieve the same effect, you can wrap your `TextInput`
 * in a `View`:
 *
 * ```ReactNativeWebPlayer
 * import React, { Component } from 'react';
 * import { AppRegistry, View, TextInput } from 'react-native';
 *
 * class UselessTextInput extends Component {
 *   render() {
 *     return (
 *       <TextInput
 *         {...this.props} // Inherit any props passed to it; e.g., multiline, numberOfLines below
 *         editable = {true}
 *         maxLength = {40}
 *       />
 *     );
 *   }
 * }
 *
 * export default class UselessTextInputMultiline extends Component {
 *   constructor(props) {
 *     super(props);
 *     this.state = {
 *       text: 'Useless Multiline Placeholder',
 *     };
 *   }
 *
 *   // If you type something in the text box that is a color, the background will change to that
 *   // color.
 *   render() {
 *     return (
 *      <View style={{
 *        backgroundColor: this.state.text,
 *        borderBottomColor: '#000000',
 *        borderBottomWidth: 1 }}
 *      >
 *        <UselessTextInput
 *          multiline = {true}
 *          numberOfLines = {4}
 *          onChangeText={(text) => this.setState({text})}
 *          value={this.state.text}
 *        />
 *      </View>
 *     );
 *   }
 * }
 *
 * // skip these lines if using Create React Native App
 * AppRegistry.registerComponent(
 *  'AwesomeProject',
 *  () => UselessTextInputMultiline
 * );
 * ```
 *
 * `TextInput` has by default a border at the bottom of its view. This border
 * has its padding set by the background image provided by the system, and it
 * cannot be changed. Solutions to avoid this is to either not set height
 * explicitly, case in which the system will take care of displaying the border
 * in the correct position, or to not display the border by setting
 * `underlineColorAndroid` to transparent.
 *
 * Note that on Android performing text selection in input can change
 * app's activity `windowSoftInputMode` param to `adjustResize`.
 * This may cause issues with components that have position: 'absolute'
 * while keyboard is active. To avoid this behavior either specify `windowSoftInputMode`
 * in AndroidManifest.xml ( https://developer.android.com/guide/topics/manifest/activity-element.html )
 * or control this param programmatically with native code.
 *
 */
function InternalTextInput(props: Props): React.Node {
  const inputRef = useRef<null | React.ElementRef<HostComponent<mixed>>>(null);

  // Android sends a "onTextChanged" event followed by a "onSelectionChanged" event, for
  // the same "most recent event count".
  // For controlled selection, that means that immediately after text is updated,
  // a controlled component will pass in the *previous* selection, even if the controlled
  // component didn't mean to modify the selection at all.
  // Therefore, we ignore selections and pass them through until the selection event has
  // been sent.
  // Note that this mitigation is NOT needed for Fabric.
  // discovered when upgrading react-hooks
  // eslint-disable-next-line react-hooks/exhaustive-deps
  let selection: ?Selection =
    props.selection == null
      ? null
      : {
          start: props.selection.start,
          end: props.selection.end ?? props.selection.start,
        };

  const [mostRecentEventCount, setMostRecentEventCount] = useState<number>(0);

  const [lastNativeText, setLastNativeText] = useState<?Stringish>(props.value);
  const [lastNativeSelectionState, setLastNativeSelection] = useState<{|
    selection: ?Selection,
    mostRecentEventCount: number,
  |}>({selection, mostRecentEventCount});

  const lastNativeSelection = lastNativeSelectionState.selection;
  const lastNativeSelectionEventCount =
    lastNativeSelectionState.mostRecentEventCount;

  if (lastNativeSelectionEventCount < mostRecentEventCount) {
    selection = null;
  }

  let viewCommands;
  if (AndroidTextInputCommands) {
    viewCommands = AndroidTextInputCommands;
  }
  // [Windows
  else if (WindowsTextInputCommands) {
    viewCommands = WindowsTextInputCommands;
  }
  // Windows]
  else {
    viewCommands =
      props.multiline === true
        ? RCTMultilineTextInputNativeCommands
        : RCTSinglelineTextInputNativeCommands;
  }

  const text =
    typeof props.value === 'string'
      ? props.value
      : typeof props.defaultValue === 'string'
      ? props.defaultValue
      : '';

  // This is necessary in case native updates the text and JS decides
  // that the update should be ignored and we should stick with the value
  // that we have in JS.
  useLayoutEffect(() => {
    const nativeUpdate = {};

    if (lastNativeText !== props.value && typeof props.value === 'string') {
      nativeUpdate.text = props.value;
      setLastNativeText(props.value);
    }

    if (
      selection &&
      lastNativeSelection &&
      (lastNativeSelection.start !== selection.start ||
        lastNativeSelection.end !== selection.end)
    ) {
      nativeUpdate.selection = selection;
      setLastNativeSelection({selection, mostRecentEventCount});
    }

    if (Object.keys(nativeUpdate).length === 0) {
      return;
    }

    if (inputRef.current != null) {
      viewCommands.setTextAndSelection(
        inputRef.current,
        mostRecentEventCount,
        text,
        selection?.start ?? -1,
        selection?.end ?? -1,
      );
    }
  }, [
    mostRecentEventCount,
    inputRef,
    props.value,
    props.defaultValue,
    lastNativeText,
    selection,
    lastNativeSelection,
    text,
    viewCommands,
  ]);

  useLayoutEffect(() => {
    const inputRefValue = inputRef.current;

    if (inputRefValue != null) {
      TextInputState.registerInput(inputRefValue);

      return () => {
        TextInputState.unregisterInput(inputRefValue);

        if (TextInputState.currentlyFocusedInput() === inputRefValue) {
          nullthrows(inputRefValue).blur();
        }
      };
    }
  }, [inputRef]);

  function clear(): void {
    if (inputRef.current != null) {
      viewCommands.setTextAndSelection(
        inputRef.current,
        mostRecentEventCount,
        '',
        0,
        0,
      );
    }
  }

  // TODO: Fix this returning true on null === null, when no input is focused
  function isFocused(): boolean {
    return TextInputState.currentlyFocusedInput() === inputRef.current;
  }

  function getNativeRef(): ?React.ElementRef<HostComponent<mixed>> {
    return inputRef.current;
  }

  const _setNativeRef = setAndForwardRef({
    getForwardedRef: () => props.forwardedRef,
    setLocalRef: ref => {
      inputRef.current = ref;

      /*
        Hi reader from the future. I'm sorry for this.

        This is a hack. Ideally we would forwardRef to the underlying
        host component. However, since TextInput has it's own methods that can be
        called as well, if we used the standard forwardRef then these
        methods wouldn't be accessible and thus be a breaking change.

        We have a couple of options of how to handle this:
        - Return a new ref with everything we methods from both. This is problematic
          because we need React to also know it is a host component which requires
          internals of the class implementation of the ref.
        - Break the API and have some other way to call one set of the methods or
          the other. This is our long term approach as we want to eventually
          get the methods on host components off the ref. So instead of calling
          ref.measure() you might call ReactNative.measure(ref). This would hopefully
          let the ref for TextInput then have the methods like `.clear`. Or we do it
          the other way and make it TextInput.clear(textInputRef) which would be fine
          too. Either way though is a breaking change that is longer term.
        - Mutate this ref. :( Gross, but accomplishes what we need in the meantime
          before we can get to the long term breaking change.
        */
      if (ref) {
        ref.clear = clear;
        ref.isFocused = isFocused;
        ref.getNativeRef = getNativeRef;
      }
    },
  });

  const _onChange = (event: ChangeEvent) => {
    const text = event.nativeEvent.text;
    props.onChange && props.onChange(event);
    props.onChangeText && props.onChangeText(text);

    if (inputRef.current == null) {
      // calling `props.onChange` or `props.onChangeText`
      // may clean up the input itself. Exits here.
      return;
    }

    setLastNativeText(text);
    // This must happen last, after we call setLastNativeText.
    // Different ordering can cause bugs when editing AndroidTextInputs
    // with multiple Fragments.
    // We must update this so that controlled input updates work.
    setMostRecentEventCount(event.nativeEvent.eventCount);
  };

  const _onSelectionChange = (event: SelectionChangeEvent) => {
    props.onSelectionChange && props.onSelectionChange(event);

    if (inputRef.current == null) {
      // calling `props.onSelectionChange`
      // may clean up the input itself. Exits here.
      return;
    }

    setLastNativeSelection({
      selection: event.nativeEvent.selection,
      mostRecentEventCount,
    });
  };

  const _onFocus = (event: FocusEvent) => {
    TextInputState.focusInput(inputRef.current);
    if (props.onFocus) {
      props.onFocus(event);
    }
  };

  const _onBlur = (event: BlurEvent) => {
    TextInputState.blurInput(inputRef.current);
    if (props.onBlur) {
      props.onBlur(event);
    }
  };

  const _onScroll = (event: ScrollEvent) => {
    props.onScroll && props.onScroll(event);
  };

  let textInput = null;

  // The default value for `blurOnSubmit` is true for single-line fields and
  // false for multi-line fields.
  const blurOnSubmit = props.blurOnSubmit ?? !props.multiline;

  const accessible = props.accessible !== false;
  const focusable = props.focusable !== false;

  const config = React.useMemo(
    () => ({
      onPress: (event: PressEvent) => {
        if (props.editable !== false) {
          if (inputRef.current != null) {
            inputRef.current.focus();
          }
        }
      },
      onPressIn: props.onPressIn,
      onPressOut: props.onPressOut,
      cancelable:
        Platform.OS === 'ios' ? !props.rejectResponderTermination : null,
    }),
    [
      props.editable,
      props.onPressIn,
      props.onPressOut,
      props.rejectResponderTermination,
    ],
  );

  // Hide caret during test runs due to a flashing caret
  // makes screenshot tests flakey
  let caretHidden = props.caretHidden;
  if (Platform.isTesting) {
    caretHidden = true;
  }

  // TextInput handles onBlur and onFocus events
  // so omitting onBlur and onFocus pressability handlers here.
  const {onBlur, onFocus, ...eventHandlers} = usePressability(config) || {};
  const eventPhase = Object.freeze({Capturing: 1, Bubbling: 3});
  const _keyDown = (event: KeyEvent) => {
    if (props.keyDownEvents && event.isPropagationStopped() !== true) {
      for (const el of props.keyDownEvents) {
        if (
          event.nativeEvent.code == el.code &&
          el.handledEventPhase == eventPhase.Bubbling
        ) {
          event.stopPropagation();
        }
      }
    }
    props.onKeyDown && props.onKeyDown(event);
  };

  const _keyUp = (event: KeyEvent) => {
    if (props.keyUpEvents && event.isPropagationStopped() !== true) {
      for (const el of props.keyUpEvents) {
        if (event.nativeEvent.code == el.code && el.handledEventPhase == 3) {
          event.stopPropagation();
        }
      }
    }
    props.onKeyUp && props.onKeyUp(event);
  };

  const _keyDownCapture = (event: KeyEvent) => {
    if (props.keyDownEvents && event.isPropagationStopped() !== true) {
      for (const el of props.keyDownEvents) {
        if (event.nativeEvent.code == el.code && el.handledEventPhase == 1) {
          event.stopPropagation();
        }
      }
    }
    props.onKeyDownCapture && props.onKeyDownCapture(event);
  };

  const _keyUpCapture = (event: KeyEvent) => {
    if (props.keyUpEvents && event.isPropagationStopped() !== true) {
      for (const el of props.keyUpEvents) {
        if (event.nativeEvent.code == el.code && el.handledEventPhase == 1) {
          event.stopPropagation();
        }
      }
    }
    props.onKeyUpCapture && props.onKeyUpCapture(event);
  };

  if (Platform.OS === 'ios') {
    const RCTTextInputView =
      props.multiline === true
        ? RCTMultilineTextInputView
        : RCTSinglelineTextInputView;

    const style =
      props.multiline === true
        ? [styles.multilineInput, props.style]
        : props.style;

    textInput = (
      <RCTTextInputView
        ref={_setNativeRef}
        {...props}
        {...eventHandlers}
        accessible={accessible}
        blurOnSubmit={blurOnSubmit}
        caretHidden={caretHidden}
        dataDetectorTypes={props.dataDetectorTypes}
        focusable={focusable}
        mostRecentEventCount={mostRecentEventCount}
        onBlur={_onBlur}
        onChange={_onChange}
        onContentSizeChange={props.onContentSizeChange}
        onFocus={_onFocus}
        onScroll={_onScroll}
        onSelectionChange={_onSelectionChange}
        onSelectionChangeShouldSetResponder={emptyFunctionThatReturnsTrue}
        selection={selection}
        style={style}
        text={text}
      />
    );
  } else if (Platform.OS === 'android') {
    const style = [props.style];
    const autoCapitalize = props.autoCapitalize || 'sentences';
    const placeholder = props.placeholder ?? '';
    let children = props.children;
    const childCount = React.Children.count(children);
    invariant(
      !(props.value != null && childCount),
      'Cannot specify both value and children.',
    );
    if (childCount > 1) {
      children = <Text>{children}</Text>;
    }

    textInput = (
      /* $FlowFixMe[prop-missing] the types for AndroidTextInput don't match up
       * exactly with the props for TextInput. This will need to get fixed */
      /* $FlowFixMe[incompatible-type] the types for AndroidTextInput don't
       * match up exactly with the props for TextInput. This will need to get
       * fixed */
      /* $FlowFixMe[incompatible-type-arg] the types for AndroidTextInput don't
       * match up exactly with the props for TextInput. This will need to get
       * fixed */
      <AndroidTextInput
        ref={_setNativeRef}
        {...props}
        {...eventHandlers}
        accessible={accessible}
        autoCapitalize={autoCapitalize}
        blurOnSubmit={blurOnSubmit}
        caretHidden={caretHidden}
        children={children}
        disableFullscreenUI={props.disableFullscreenUI}
        focusable={focusable}
        mostRecentEventCount={mostRecentEventCount}
        onBlur={_onBlur}
        onChange={_onChange}
        onFocus={_onFocus}
        /* $FlowFixMe[prop-missing] the types for AndroidTextInput don't match
         * up exactly with the props for TextInput. This will need to get fixed
         */
        /* $FlowFixMe[incompatible-type-arg] the types for AndroidTextInput
         * don't match up exactly with the props for TextInput. This will need
         * to get fixed */
        onScroll={_onScroll}
        onSelectionChange={_onSelectionChange}
        placeholder={placeholder}
        selection={selection}
        style={style}
        text={text}
        textBreakStrategy={props.textBreakStrategy}
      />
    );
  } // [Windows
  else if (Platform.OS === 'windows') {
    textInput = (
      <WindowsTextInput
        ref={_setNativeRef}
        {...props}
        dataDetectorTypes={props.dataDetectorTypes}
        mostRecentEventCount={mostRecentEventCount}
        onBlur={_onBlur}
        onChange={_onChange}
        onContentSizeChange={props.onContentSizeChange}
        onFocus={_onFocus}
        onScroll={_onScroll}
        onSelectionChange={_onSelectionChange}
        onSelectionChangeShouldSetResponder={emptyFunctionThatReturnsTrue}
        selection={selection}
        text={text}
        onKeyDown={_keyDown}
        onKeyDownCapture={_keyDownCapture}
        onKeyUp={_keyUp}
        onKeyUpCapture={_keyUpCapture}
      />
    );
  } // Windows]
  return (
    <TextAncestor.Provider value={true}>{textInput}</TextAncestor.Provider>
  );
}

const ExportedForwardRef: React.AbstractComponent<
  React.ElementConfig<typeof InternalTextInput>,
  React.ElementRef<HostComponent<mixed>> & ImperativeMethods,
> = React.forwardRef(function TextInput(
  {
    allowFontScaling = true,
    rejectResponderTermination = true,
    underlineColorAndroid = 'transparent',
    ...restProps
  },
  forwardedRef: ReactRefSetter<
    React.ElementRef<HostComponent<mixed>> & ImperativeMethods,
  >,
) {
  return (
    <InternalTextInput
      allowFontScaling={allowFontScaling}
      rejectResponderTermination={rejectResponderTermination}
      underlineColorAndroid={underlineColorAndroid}
      {...restProps}
      forwardedRef={forwardedRef}
    />
  );
});

// TODO: Deprecate this
ExportedForwardRef.propTypes = DeprecatedTextInputPropTypes;

// $FlowFixMe[prop-missing]
ExportedForwardRef.State = {
  currentlyFocusedInput: TextInputState.currentlyFocusedInput,

  currentlyFocusedField: TextInputState.currentlyFocusedField,
  focusTextInput: TextInputState.focusTextInput,
  blurTextInput: TextInputState.blurTextInput,
};

type TextInputComponentStatics = $ReadOnly<{|
  State: $ReadOnly<{|
    currentlyFocusedInput: typeof TextInputState.currentlyFocusedInput,
    currentlyFocusedField: typeof TextInputState.currentlyFocusedField,
    focusTextInput: typeof TextInputState.focusTextInput,
    blurTextInput: typeof TextInputState.blurTextInput,
  |}>,
  propTypes: typeof DeprecatedTextInputPropTypes,
|}>;

const styles = StyleSheet.create({
  multilineInput: {
    // This default top inset makes RCTMultilineTextInputView seem as close as possible
    // to single-line RCTSinglelineTextInputView defaults, using the system defaults
    // of font size 17 and a height of 31 points.
    paddingTop: 5,
  },
});

// $FlowFixMe[unclear-type] Unclear type. Using `any` type is not safe.
module.exports = ((ExportedForwardRef: any): React.AbstractComponent<
  React.ElementConfig<typeof InternalTextInput>,
  $ReadOnly<{|
    ...React.ElementRef<HostComponent<mixed>>,
    ...ImperativeMethods,
  |}>,
> &
  TextInputComponentStatics);
