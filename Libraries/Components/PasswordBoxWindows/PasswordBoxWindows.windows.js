/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule PasswordBoxWindows
 * @flow
 */
'use strict';

var DocumentSelectionState = require('DocumentSelectionState');
var EventEmitter = require('EventEmitter');
var NativeMethodsMixin = require('react/lib/NativeMethodsMixin');
var PropTypes = require('react/lib/ReactPropTypes');
var React = require('React');
var ReactNative = require('react/lib/ReactNative');
var StyleSheet = require('StyleSheet');
var Text = require('Text');
var TextInputState = require('TextInputState');
var TimerMixin = require('react-timer-mixin');
var TouchableWithoutFeedback = require('TouchableWithoutFeedback');
var View = require('View');

var requireNativeComponent = require('requireNativeComponent');

var NativePasswordBox = requireNativeComponent('PasswordBoxWindows', null);

type Event = Object;

/**
 * A foundational component for inputting passwords into the app via a
 * keyboard. Props provide configurability for several features, such as
 * placeholder text, and different keyboardvtypes, such as a numeric keypad.
 *
 * The simplest use case is to plop down a `PasswordBoxWindows` and subscribe to the
 * `onChangeText` events to read the user input. There are also other events,
 * such as `onSubmitEditing` and `onFocus` that can be subscribed to. A simple
 * example:
 *
 * ```
 *   <PasswordBox
 *     style={{height: 40, borderColor: 'gray', borderWidth: 1}}
 *     onChangeText={(text) => this.setState({text})}
 *     value={this.state.text}
 *   />
 * ```
 */
var PasswordBoxWindows = React.createClass({
  statics: {
    /* TODO(brentvatne) docs are needed for this */
    State: TextInputState,
  },

  propTypes: {
    ...View.propTypes,
    /**
     * If false, disables auto-correct. The default value is true.
     */
    autoFocus: PropTypes.bool,
    /**
     * If false, text is not editable. The default value is true.
     */
    editable: PropTypes.bool,
    /**
     * Determines which keyboard to open, e.g.`numeric`.
     *
     * The following values work across platforms:
     *
     * - default
     * - numeric
     * - email-address
     */
    keyboardType: PropTypes.oneOf([
      // Cross-platform
      'default',
      'email-address',
      'numeric',
      'phone-pad',
      // iOS-only
      'ascii-capable',
      'numbers-and-punctuation',
      'url',
      'number-pad',
      'name-phone-pad',
      'decimal-pad',
      'twitter',
      'web-search',
    ]),
    /**
     * Determines the color of the keyboard.
     * @platform ios
     */
    keyboardAppearance: PropTypes.oneOf([
      'default',
      'light',
      'dark',
    ]),
    /**
     * Determines how the return key should look. On Android you can also use
     * `returnKeyLabel`.
     *
     * The following values work across platforms:
     *
     * - done
     * - go
     * - next
     * - search
     * - send
     *
     * The following values work on Android only:
     *
     * - none
     * - previous
     *
     * The following values work on iOS only:
     *
     * - default
     * - emergency-call
     * - google
     * - join
     * - route
     * - yahoo
     */
    returnKeyType: PropTypes.oneOf([
      // Cross-platform
      'done',
      'go',
      'next',
      'search',
      'send',
      // Android-only
      'none',
      'previous',
      // iOS-only
      'default',
      'emergency-call',
      'google',
      'join',
      'route',
      'yahoo',
    ]),
    /**
     * Sets the return key to the label. Use it instead of `returnKeyType`.
     * @platform android
     */
     returnKeyLabel: PropTypes.string,
    /**
     * Limits the maximum number of characters that can be entered. Use this
     * instead of implementing the logic in JS to avoid flicker.
     */
    maxLength: PropTypes.number,
    /**
     * If true, the keyboard disables the return key when there is no text and
     * automatically enables it when there is text. The default value is false.
     * @platform ios
     */
    enablesReturnKeyAutomatically: PropTypes.bool,
    /**
     * Callback that is called when the text input is blurred
     */
    onBlur: PropTypes.func,
    /**
     * Callback that is called when the text input is focused
     */
    onFocus: PropTypes.func,
    /**
     * Callback that is called when the text input's text changes.
     */
    onChange: PropTypes.func,
    /**
     * Callback that is called when the text input's text changes.
     * Changed text is passed as an argument to the callback handler.
     */
    onChangeText: PropTypes.func,
    /**
     * Callback that is called when text input ends.
     */
    onEndEditing: PropTypes.func,
    /**
     * Callback that is called when the text input's submit button is pressed.
     * Invalid if multiline={true} is specified.
     */
    onSubmitEditing: PropTypes.func,
    /**
     * Callback that is called when a key is pressed.
     * Pressed key value is passed as an argument to the callback handler.
     * Fires before onChange callbacks.
     * @platform ios
     */
    onKeyPress: PropTypes.func,
    /**
     * Invoked on mount and layout changes with `{x, y, width, height}`.
     */
    onLayout: PropTypes.func,
    /**
     * The string used for the password character mask
     */
    passwordChar: PropTypes.string,
    /**
     * The password reveal mode, either "Hidden", "Peek", or "Visible"
     */
    passwordRevealMode: PropTypes.string,
    /**
     * The string that will be rendered before text input has been entered
     */
    placeholder: PropTypes.string,
    /**
     * The text color of the placeholder string
     */
    placeholderTextColor: PropTypes.string,
    /**
    * The highlight (and cursor on ios) color of the text input
    */
    selectionColor: PropTypes.string,
    /**
     * See DocumentSelectionState.js, some state that is responsible for
     * maintaining selection information for a document
     * @platform ios
     */
    selectionState: PropTypes.instanceOf(DocumentSelectionState),
    /**
     * The value to show for the text input. TextInput is a controlled
     * component, which means the native value will be forced to match this
     * value prop if provided. For most uses this works great, but in some
     * cases this may cause flickering - one common cause is preventing edits
     * by keeping value the same. In addition to simply setting the same value,
     * either set `editable={false}`, or set/update `maxLength` to prevent
     * unwanted edits without flicker.
     */
    value: PropTypes.string,
    /**
     * Provides an initial value that will change when the user starts typing.
     * Useful for simple use-cases where you don't want to deal with listening
     * to events and updating the value prop to keep the controlled state in sync.
     */
    defaultValue: PropTypes.string,
    /**
     * When the clear button should appear on the right side of the text view
     * @platform ios
     */
    clearButtonMode: PropTypes.oneOf([
      'never',
      'while-editing',
      'unless-editing',
      'always',
    ]),
    /**
     * If true, clears the text field automatically when editing begins
     * @platform ios
     */
    clearTextOnFocus: PropTypes.bool,
    /**
     * If true, all text will automatically be selected on focus
     */
    selectTextOnFocus: PropTypes.bool,
    /**
     * If true, the text field will blur when submitted.
     * The default value is true for single-line fields and false for
     * multiline fields. Note that for multiline fields, setting blurOnSubmit
     * to true means that pressing return will blur the field and trigger the
     * onSubmitEditing event instead of inserting a newline into the field.
     */
    blurOnSubmit: PropTypes.bool,
    /**
     * Styles
     */
    style: Text.propTypes.style,
    /**
     * The color of the textInput underline.
     * @platform android
     */
    underlineColorAndroid: PropTypes.string,
  },

  /**
   * `NativeMethodsMixin` will look for this when invoking `setNativeProps`. We
   * make `this` look like an actual native component class.
   */
  mixins: [NativeMethodsMixin, TimerMixin],

  viewConfig: NativePasswordBox.viewConfig,

  /**
   * Returns if the input is currently focused.
   */
  isFocused: function(): boolean {
    return TextInputState.currentlyFocusedField() ===
      ReactNative.findNodeHandle(this.refs.input);
  },

  contextTypes: {
    onFocusRequested: React.PropTypes.func,
    focusEmitter: React.PropTypes.instanceOf(EventEmitter),
  },

  _focusSubscription: (undefined: ?Function),
  _lastNativeText: (undefined: ?string),

  componentDidMount: function() {
    this._lastNativeText = this.props.value;
    if (!this.context.focusEmitter) {
      if (this.props.autoFocus) {
        this.requestAnimationFrame(this.focus);
      }
      return;
    }
    this._focusSubscription = this.context.focusEmitter.addListener(
      'focus',
      (el) => {
        if (this === el) {
          this.requestAnimationFrame(this.focus);
        } else if (this.isFocused()) {
          this.blur();
        }
      }
    );
    if (this.props.autoFocus) {
      this.context.onFocusRequested(this);
    }
  },

  componentWillUnmount: function() {
    this._focusSubscription && this._focusSubscription.remove();
    if (this.isFocused()) {
      this.blur();
    }
  },

  getChildContext: function(): Object {
    return {isInAParentText: true};
  },

  childContextTypes: {
    isInAParentText: React.PropTypes.bool
  },

  /**
   * Removes all text from the input.
   */
  clear: function() {
    this.setNativeProps({text: ''});
  },

  _getText: function(): ?string {
    return typeof this.props.value === 'string' ?
      this.props.value :
      this.props.defaultValue;
  },

  render: function() {
    var textContainer =
      <NativePasswordBox
        ref="input"
        style={[this.props.style]}
        keyboardType={this.props.keyboardType}
        maxLength={this.props.maxLength}
        onFocus={this._onFocus}
        onBlur={this._onBlur}
        onChange={this._onChange}
        onEndEditing={this.props.onEndEditing}
        onSubmitEditing={this.props.onSubmitEditing}
        clearTextOnFocus={this.props.clearTextOnFocus}
        selectTextOnFocus={this.props.selectTextOnFocus}
        onLayout={this.props.onLayout}
        passwordChar={this.props.passwordChar}
        passwordRevealMode={this.props.passwordRevealMode}
        placeholder={this.props.placeholder}
        selectionColor={this.props.selectionColor}
        text={this._getText()}
        editable={this.props.editable}
      />;

    return (
      <TouchableWithoutFeedback
        onPress={this._onPress}
        rejectResponderTermination={true}
        accessible={this.props.accessible}
        accessibilityLabel={this.props.accessibilityLabel}
        accessibilityTraits={this.props.accessibilityTraits}
        testID={this.props.testID}>
        {textContainer}
      </TouchableWithoutFeedback>
    );
  },


  _onFocus: function(event: Event) {
    if (this.props.onFocus) {
      this.props.onFocus(event);
    }

    if (this.props.selectionState) {
      this.props.selectionState.focus();
    }
  },

  _onPress: function(event: Event) {
    if (this.props.editable || this.props.editable === undefined) {
      this.focus();
    }
  },

  _onChange: function(event: Event) {
    // Make sure to fire the mostRecentEventCount first so it is already set on
    // native when the text value is set.
    this.refs.input.setNativeProps({
      mostRecentEventCount: event.nativeEvent.eventCount,
    });

    var text = event.nativeEvent.text;
    this.props.onChange && this.props.onChange(event);
    this.props.onChangeText && this.props.onChangeText(text);

    if (!this.refs.input) {
      // calling `this.props.onChange` or `this.props.onChangeText`
      // may clean up the input itself. Exits here.
      return;
    }

    this._lastNativeText = text;
    this.forceUpdate();
  },

  componentDidUpdate: function () {
    // This is necessary in case native updates the text and JS decides
    // that the update should be ignored and we should stick with the value
    // that we have in JS.
    if (this._lastNativeText !== this.props.value && typeof this.props.value === 'string') {
      this.refs.input.setNativeProps({
        text: this.props.value,
      });
    }
  },

  _onBlur: function(event: Event) {
    this.blur();
    if (this.props.onBlur) {
      this.props.onBlur(event);
    }

    if (this.props.selectionState) {
      this.props.selectionState.blur();
    }
  },

  _onTextInput: function(event: Event) {
    this.props.onTextInput && this.props.onTextInput(event);
  },
});

var styles = StyleSheet.create({
  input: {
    alignSelf: 'stretch',
  },
});

module.exports = PasswordBoxWindows;
