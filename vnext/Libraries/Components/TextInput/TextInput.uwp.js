/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * Portions copyright for react-native-windows:
 *
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @flow
 */
'use strict';

const EventEmitter = require('EventEmitter');
const NativeMethodsMixin = require('NativeMethodsMixin');
const React = require('React');
const createReactClass = require('create-react-class');
const PropTypes = require('prop-types');
const ReactNative = require('ReactNative');
const StyleSheet = require('StyleSheet');
const TextAncestor = require('TextAncestor');
const TextInputState = require('TextInputState');
/* $FlowFixMe(>=0.54.0 site=react_native_oss) This comment suppresses an error
 * found when Flow v0.54 was deployed. To see the error delete this comment and
 * run Flow. */
const TimerMixin = require('react-timer-mixin');
import type {SyntheticEvent, ScrollEvent} from 'CoreEventTypes';
import type {PressEvent} from 'CoreEventTypes';

const requireNativeComponent = require('requireNativeComponent');

var RCTTextInput = requireNativeComponent('RCTTextInput'); // TODO(windows ISS)

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

/**
 * A foundational component for inputting text into the app via a
 * keyboard. Props provide configurability for several features, such as
 * auto-correction, auto-capitalization, placeholder text, and different keyboard
 * types, such as a numeric keypad.
 *
 * The simplest use case is to plop down a `TextInput` and subscribe to the
 * `onChangeText` events to read the user input. There are also other events,
 * such as `onFocus` that can be subscribed to. A simple
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
 */

const TextInput = createReactClass({
  displayName: 'TextInput',
  statics: {
    State: {
      currentlyFocusedField: TextInputState.currentlyFocusedField,
      focusTextInput: TextInputState.focusTextInput,
      blurTextInput: TextInputState.blurTextInput,
    },
  },

  getDefaultProps(): Object {
    return {
      allowFontScaling: true,
    };
  },
  /**
   * `NativeMethodsMixin` will look for this when invoking `setNativeProps`. We
   * make `this` look like an actual native component class.
   */
  mixins: [NativeMethodsMixin, TimerMixin],

  /**
   * Returns `true` if the input is currently focused; `false` otherwise.
   */
  isFocused: function(): boolean {
    return (
      TextInputState.currentlyFocusedField() ===
      ReactNative.findNodeHandle(this._inputRef)
    );
  },

  /**
   * Returns the native `TextView` node.
   */
  getTextViewHandle: function(): any {
    // TODO(OSS Candidate ISS#2710739)
    return ReactNative.findNodeHandle(this._inputRef);
  },

  _inputRef: (undefined: any),
  _focusSubscription: (undefined: ?Function),
  _lastNativeText: (undefined: ?string),
  _lastNativeSelection: (undefined: ?Selection),
  _rafId: (null: ?AnimationFrameID),

  componentDidMount: function() {
    this._lastNativeText = this.props.value;
    const tag = ReactNative.findNodeHandle(this._inputRef);
    if (tag != null) {
      // tag is null only in unit tests
      TextInputState.registerInput(tag);
    }

    if (this.context.focusEmitter) {
      this._focusSubscription = this.context.focusEmitter.addListener(
        'focus',
        el => {
          if (this === el) {
            this._rafId = requestAnimationFrame(this.focus);
          } else if (this.isFocused()) {
            this.blur();
          }
        },
      );
      if (this.props.autoFocus) {
        this.context.onFocusRequested(this);
      }
    } else {
      if (this.props.autoFocus) {
        this._rafId = requestAnimationFrame(this.focus);
      }
    }
  },

  componentWillUnmount: function() {
    this._focusSubscription && this._focusSubscription.remove();
    if (this.isFocused()) {
      this.blur();
    }
    if (this._rafId != null) {
      cancelAnimationFrame(this._rafId);
    }
  },

  contextTypes: {
    onFocusRequested: PropTypes.func,
    focusEmitter: PropTypes.instanceOf(EventEmitter),
  },

  /**
   * Removes all text from the `TextInput`.
   */
  clear: function() {
    this.setNativeProps({text: ''});
  },

  render: function() {
    var props = Object.assign({}, this.props);
    props.style = [styles.input, this.props.style];

    return (
      <TextAncestor.Provider value={true}>
        <RCTTextInput
          ref={this._setNativeRef}
          {...props}
          mostRecentEventCount={0}
          onChange={this._onChange}
          onBlur={this._onBlur}
          onFocus={this._onFocus}
          onSelectionChange={this._onSelectionChange}
          text={this._getText()}
        />
      </TextAncestor.Provider>
    );
  },

  _getText: function(): ?string {
    return typeof this.props.value === 'string'
      ? this.props.value
      : typeof this.props.defaultValue === 'string'
        ? this.props.defaultValue
        : '';
  },

  _setNativeRef: function(ref: any) {
    this._inputRef = ref;
  },

  _onFocus: function(event: FocusEvent) {
    // [TODO(android ISS)
    // Set the focused TextInput field info in TextInputState.
    // Delaying this to onFocus native event ensures that -
    // 1. The state is updated only after the native code completes setting focus on the view
    // 2. In case the focus is moving from one TextInput(A) to another TextInput(B), the state of
    //    A needs to be updated (blurred) before info about B is updated in TestInputState.
    TextInputState.setFocusedTextInput(
      ReactNative.findNodeHandle(this._inputRef),
    ); // ]TODO(android ISS)
    if (this.props.onFocus) {
      this.props.onFocus(event);
    }
  },

  _onPress: function(event: PressEvent) {
    if (this.props.editable || this.props.editable === undefined) {
      this.focus();
    }
  },

  _onChange: function(event: ChangeEvent) {
    // Make sure to fire the mostRecentEventCount first so it is already set on
    // native when the text value is set.
    if (this._inputRef && this._inputRef.setNativeProps) {
      this._inputRef.setNativeProps({
        mostRecentEventCount: event.nativeEvent.eventCount,
      });
    }

    const text = event.nativeEvent.text;
    this.props.onChange && this.props.onChange(event);
    this.props.onChangeText && this.props.onChangeText(text);

    if (!this._inputRef) {
      // calling `this.props.onChange` or `this.props.onChangeText`
      // may clean up the input itself. Exits here.
      return;
    }

    this._lastNativeText = text;
    this.forceUpdate();
  },

  _onSelectionChange: function(event: SelectionChangeEvent) {
    this.props.onSelectionChange && this.props.onSelectionChange(event);

    if (!this._inputRef) {
      // calling `this.props.onSelectionChange`
      // may clean up the input itself. Exits here.
      return;
    }

    this._lastNativeSelection = event.nativeEvent.selection;

    if (this.props.selection) {
      this.forceUpdate();
    }
  },

  componentDidUpdate: function() {
    // This is necessary in case native updates the text and JS decides
    // that the update should be ignored and we should stick with the value
    // that we have in JS.
    const nativeProps = {};

    if (
      this._lastNativeText !== this.props.value &&
      typeof this.props.value === 'string'
    ) {
      nativeProps.text = this.props.value;
    }

    // Selection is also a controlled prop, if the native value doesn't match
    // JS, update to the JS value.
    const {selection} = this.props;
    if (
      this._lastNativeSelection &&
      selection &&
      (this._lastNativeSelection.start !== selection.start ||
        this._lastNativeSelection.end !== selection.end)
    ) {
      nativeProps.selection = this.props.selection;
    }

    if (
      Object.keys(nativeProps).length > 0 &&
      this._inputRef &&
      this._inputRef.setNativeProps
    ) {
      this._inputRef.setNativeProps(nativeProps);
    }
  },

  _onBlur: function(event: Event) {
    // [TODO(android ISS)
    // Set the focused TextInput field info in TextInputState.
    // Delaying this to onBlur native event ensures that -
    // 1. The state is updated only after the native code completes clearing focus on the view
    // 2. In case the focus is moving from one TextInput(A) to another TextInput(B), the state of
    //    A needs to be updated (blurred) before info about B is updated in TestInputState.
    TextInputState.clearFocusedTextInput(
      ReactNative.findNodeHandle(this._inputRef),
    ); // ]TODO(android ISS)
    if (this.props.onBlur) {
      this.props.onBlur(event);
    }
  },

  _onTextInput: function(event: TextInputEvent) {
    this.props.onTextInput && this.props.onTextInput(event);
  },

  _onScroll: function(event: ScrollEvent) {
    this.props.onScroll && this.props.onScroll(event);
  },
});

const styles = StyleSheet.create({
  multilineInput: {
    // This default top inset makes RCTMultilineTextInputView seem as close as possible
    // to single-line RCTSinglelineTextInputView defaults, using the system defaults
    // of font size 17 and a height of 31 points.
    paddingTop: 5,
  },
});

module.exports = TextInput;
