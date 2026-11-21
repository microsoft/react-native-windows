/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<c0db9bc834f6c1b46f5401652a6afdc6>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\TextInput\TextInput.js
 */

import type { HostComponent } from "../../../src/private/types/HostComponent";
import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { BlurEvent, FocusEvent } from "../../Types/CoreEventTypes";
import type { AutoCapitalize, EnterKeyHintType, EnterKeyHintTypeAndroid, EnterKeyHintTypeIOS, EnterKeyHintTypeOptions, InputModeOptions, KeyboardType, KeyboardTypeAndroid, KeyboardTypeIOS, KeyboardTypeOptions, ReturnKeyType, ReturnKeyTypeAndroid, ReturnKeyTypeIOS, ReturnKeyTypeOptions, SubmitBehavior, TextContentType, TextInputAndroidProps, TextInputBlurEvent, TextInputChangeEvent, TextInputContentSizeChangeEvent, TextInputEditingEvent, TextInputEndEditingEvent, TextInputEvent, TextInputFocusEvent, TextInputInstance, TextInputIOSProps, TextInputKeyPressEvent, TextInputProps, TextInputSelectionChangeEvent, TextInputSubmitEditingEvent, TextInputType, TextInputWindowsProps } from "./TextInput.flow";
import * as React from "react";
import { type WindowsTextInputNativeProps } from "./WindowsTextInputNativeComponent";
export type { AutoCapitalize, BlurEvent, EnterKeyHintType, EnterKeyHintTypeAndroid, EnterKeyHintTypeIOS, EnterKeyHintTypeOptions, FocusEvent, InputModeOptions, KeyboardType, KeyboardTypeAndroid, KeyboardTypeIOS, KeyboardTypeOptions, ReturnKeyType, ReturnKeyTypeAndroid, ReturnKeyTypeIOS, ReturnKeyTypeOptions, SubmitBehavior, TextContentType, TextInputAndroidProps, TextInputBlurEvent, TextInputChangeEvent, TextInputContentSizeChangeEvent, TextInputEditingEvent, TextInputEndEditingEvent, TextInputEvent, TextInputFocusEvent, TextInputIOSProps, TextInputKeyPressEvent, TextInputProps, TextInputSelectionChangeEvent, TextInputInstance, TextInputSubmitEditingEvent, TextInputWindowsProps };
type TextInputStateType = Readonly<{
  /**
   * @deprecated Use currentlyFocusedInput
   * Returns the ID of the currently focused text field, if one exists
   * If no text field is focused it returns null
   */
  currentlyFocusedField: () => number | undefined;
  /**
   * Returns the ref of the currently focused text field, if one exists
   * If no text field is focused it returns null
   */
  currentlyFocusedInput: () => HostInstance | undefined;
  /**
   * @param textField ref of the text field to focus
   * Focuses the specified text field
   * noop if the text field was already focused
   */
  focusTextInput: (textField: HostInstance | undefined) => void;
  /**
   * @param textField ref of the text field to focus
   * Unfocuses the specified text field
   * noop if it wasn't focused
   */
  blurTextInput: (textField: HostInstance | undefined) => void;
}>;
export declare function InternalTextInputWithCustomType(props: TextInputProps, NativeComponent: HostComponent<WindowsTextInputNativeProps>): React.ReactNode;
export type TextInputComponentStatics = Readonly<{
  State: TextInputStateType;
}>;
/**
 * A foundational component for inputting text into the app via a
 * keyboard. Props provide configurability for several features, such as
 * auto-correction, auto-capitalization, placeholder text, and different keyboard
 * types, such as a numeric keypad.
 *
 * The simplest use case is to plop down a `TextInput` and subscribe to the
 * `onChangeText` events to read the user input. There are also other events,
 * such as `onSubmitEditing` and `onFocus` that can be subscribed to.
 *
 * Example:
 *
 * ```tsx
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
 * Example:
 *
 * ```tsx
 * import React, { Component } from 'react';
 * import { AppRegistry, View, TextInput } from 'react-native';
 *
 * class UselessTextInput extends Component {
 *   render() {
 *     return (
 *       <TextInput
 *         {...this.props} // Inherit any props passed to it; e.g., multiline, numberOfLines below
 *         editable={true}
 *         maxLength={40}
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
 *          multiline={true}
 *          numberOfLines={4}
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
 * @see https://reactnative.dev/docs/textinput
 */
declare const $$TextInput: TextInputType;
declare type $$TextInput = typeof $$TextInput;
export default $$TextInput;
