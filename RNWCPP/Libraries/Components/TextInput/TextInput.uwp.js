/**
 *
 * @flow
 * @format
 */
'use strict';

const ColorPropType = require('ColorPropType');
const EventEmitter = require('EventEmitter');
const NativeMethodsMixin = require('NativeMethodsMixin');
const React = require('React');
const createReactClass = require('create-react-class');
const PropTypes = require('prop-types');
const ReactNative = require('ReactNative');
const StyleSheet = require('StyleSheet');
const Text = require('Text');
const TextAncestor = require('TextAncestor');
const TextInputState = require('TextInputState');
/* $FlowFixMe(>=0.54.0 site=react_native_oss) This comment suppresses an error
 * found when Flow v0.54 was deployed. To see the error delete this comment and
 * run Flow. */
const TimerMixin = require('react-timer-mixin');
const ViewPropTypes = require('ViewPropTypes');

const requireNativeComponent = require('requireNativeComponent');
/* $FlowFixMe(>=0.54.0 site=react_native_oss) This comment suppresses an error
 * found when Flow v0.54 was deployed. To see the error delete this comment and
 * run Flow. */
const warning = require('fbjs/lib/warning');


var RCTTextInput = requireNativeComponent('RCTTextInput'); // TODO(windows ISS)

type Event = Object;
type Selection = {
  start: number,
  end?: number,
};

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

  propTypes: {
    ...ViewPropTypes,
    /**
     * If `false`, disables spell-check style (i.e. red underlines).
     * The default value is inherited from `autoCorrect`.
     * @platform ios
     */
    spellCheck: PropTypes.bool,
    /**
     * If `true`, focuses the input on `componentDidMount`.
     * The default value is `false`.
     */
    autoFocus: PropTypes.bool,
    /**
     * Specifies whether fonts should scale to respect Text Size accessibility settings. The
     * default is `true`.
     */
    allowFontScaling: PropTypes.bool,
    /**
     * If `false`, text is not editable. The default value is `true`.
     */
    editable: PropTypes.bool,
    /**
     * Limits the maximum number of characters that can be entered. Use this
     * instead of implementing the logic in JS to avoid flicker.
     */
    maxLength: PropTypes.number,
    /**
     * If `true`, the text input can be multiple lines.
     * The default value is `false`.
     */
    multiline: PropTypes.bool,
    /**
     * Callback that is called when the text input is blurred.
     */
    onBlur: PropTypes.func,
    /**
     * Callback that is called when the text input is focused.
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
     * Callback that is called when the text input's content size changes.
     * This will be called with
     * `{ nativeEvent: { contentSize: { width, height } } }`.
     *
     * Only called for multiline text inputs.
     */
    onContentSizeChange: PropTypes.func,
    /**
     * Callback that is called when text input ends.
     */
    onEndEditing: PropTypes.func,
    /**
     * Callback that is called when the text input selection is changed.
     * This will be called with
     * `{ nativeEvent: { selection: { start, end } } }`.
     */
    onSelectionChange: PropTypes.func,
    /**
     * Callback that is called when a key is pressed.
     * This will be called with `{ nativeEvent: { key: keyValue } }`
     * where `keyValue` is `'Enter'` or `'Backspace'` for respective keys and
     * the typed-in character otherwise including `' '` for space.
     * Fires before `onChange` callbacks.
     */
    onKeyPress: PropTypes.func,
    /**
     * Invoked on mount and layout changes with `{x, y, width, height}`.
     */
    onLayout: PropTypes.func,
    /**
     * Invoked on content scroll with `{ nativeEvent: { contentOffset: { x, y } } }`.
     * May also contain other properties from ScrollEvent but on Android contentSize
     * is not provided for performance reasons.
     */
    onScroll: PropTypes.func,
    /**
     * The string that will be rendered before text input has been entered.
     */
    placeholder: PropTypes.string,
    /**
     * The text color of the placeholder string.
     */
    placeholderTextColor: ColorPropType,
    /**
     * If `false`, scrolling of the text view will be disabled.
     * The default value is `true`. Does only work with 'multiline={true}'.
     */
    scrollEnabled: PropTypes.bool, // TODO(OSS Candidate ISS#2710739)
    /**
     * If `true`, the text input obscures the text entered so that sensitive text
     * like passwords stay secure. The default value is `false`. Does not work with 'multiline={true}'.
     */
    secureTextEntry: PropTypes.bool,
    /**
     * The highlight and cursor color of the text input.
     */
    selectionColor: ColorPropType,
    /**
     * The start and end of the text input's selection. Set start and end to
     * the same value to position the cursor.
     */
    selection: PropTypes.shape({
      start: PropTypes.number.isRequired,
      end: PropTypes.number,
    }),
    /**
     * The value to show for the text input. `TextInput` is a controlled
     * component, which means the native value will be forced to match this
     * value prop if provided. For most uses, this works great, but in some
     * cases this may cause flickering - one common cause is preventing edits
     * by keeping value the same. In addition to simply setting the same value,
     * either set `editable={false}`, or set/update `maxLength` to prevent
     * unwanted edits without flicker.
     */
    value: PropTypes.string,
    /**
     * Provides an initial value that will change when the user starts typing.
     * Useful for simple use-cases where you do not want to deal with listening
     * to events and updating the value prop to keep the controlled state in sync.
     */
    defaultValue: PropTypes.string,
    /**
     * If `true`, clears the text field automatically when editing begins.
     * @platform ios
     */
    clearTextOnFocus: PropTypes.bool,
    /**
     * If `true`, all text will automatically be selected on focus.
     */
    selectTextOnFocus: PropTypes.bool,
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
    style: Text.propTypes.style,
    /**
     * If `true`, caret is hidden. The default value is `false`.
     * This property is supported only for single-line TextInput component on iOS.
     */
    caretHidden: PropTypes.bool,
    /*
     * If `true`, contextMenuHidden is hidden. The default value is `false`.
     */
    contextMenuHidden: PropTypes.bool,
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
  isFocused: function (): boolean {
    return (
      TextInputState.currentlyFocusedField() ===
      ReactNative.findNodeHandle(this._inputRef)
    );
  },

  /**
   * Returns the native `TextView` node.
   */
  getTextViewHandle: function (): any { // TODO(OSS Candidate ISS#2710739)
    return ReactNative.findNodeHandle(this._inputRef);
  },

  _inputRef: (undefined: any),
  _focusSubscription: (undefined: ?Function),
  _lastNativeText: (undefined: ?string),
  _lastNativeSelection: (undefined: ?Selection),

  componentDidMount: function () {
    this._lastNativeText = this.props.value;
    if (!this.context.focusEmitter) {
      if (this.props.autoFocus) {
        this.requestAnimationFrame(this.focus);
      }
      return;
    }
    this._focusSubscription = this.context.focusEmitter.addListener(
      'focus',
      el => {
        if (this === el) {
          this.requestAnimationFrame(this.focus);
        } else if (this.isFocused()) {
          this.blur();
        }
      },
    );
    if (this.props.autoFocus) {
      this.context.onFocusRequested(this);
    }
  },

  componentWillUnmount: function () {
    this._focusSubscription && this._focusSubscription.remove();
    if (this.isFocused()) {
      this.blur();
    }
  },

  contextTypes: {
    onFocusRequested: PropTypes.func,
    focusEmitter: PropTypes.instanceOf(EventEmitter),
  },

  /**
   * Removes all text from the `TextInput`.
   */
  clear: function () {
    this.setNativeProps({ text: '' });
  },

  render: function () {
    var props = Object.assign({}, this.props);
    props.style = [styles.input, this.props.style];

    return (
      <TextAncestor.Provider value={true}>
      <RCTTextInput
        ref={this._setNativeRef}
        {...props}
        onChange={this._onChange}
        onBlur={this._onBlur}
        onFocus={this._onFocus}
        onSelectionChange={this._onSelectionChange}
        text={this._getText()}
      />
      </TextAncestor.Provider>
    );
  },

  _getText: function (): ?string {
    return typeof this.props.value === 'string'
      ? this.props.value
      : typeof this.props.defaultValue === 'string'
        ? this.props.defaultValue
        : '';
  },

  _setNativeRef: function (ref: any) {
    this._inputRef = ref;
  },

  _onFocus: function (event: Event) {
    // [TODO(android ISS)
    // Set the focused TextInput field info in TextInputState.
    // Delaying this to onFocus native event ensures that -
    // 1. The state is updated only after the native code completes setting focus on the view
    // 2. In case the focus is moving from one TextInput(A) to another TextInput(B), the state of
    //    A needs to be updated (blurred) before info about B is updated in TestInputState.
    TextInputState.setFocusedTextInput(ReactNative.findNodeHandle(this._inputRef)); // ]TODO(android ISS)
    if (this.props.onFocus) {
      this.props.onFocus(event);
    }
  },

  _onPress: function (event: Event) {
    if (this.props.editable || this.props.editable === undefined) {
      this.focus();
    }
  },

  _onChange: function (event: Event) {
    // Make sure to fire the mostRecentEventCount first so it is already set on
    // native when the text value is set.
    if (this._inputRef) {
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

  _onSelectionChange: function (event: Event) {
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

  componentDidUpdate: function () {
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
    const { selection } = this.props;
    if (
      this._lastNativeSelection &&
      selection &&
      (this._lastNativeSelection.start !== selection.start ||
        this._lastNativeSelection.end !== selection.end)
    ) {
      nativeProps.selection = this.props.selection;
    }

    if (Object.keys(nativeProps).length > 0 && this._inputRef) {
      this._inputRef.setNativeProps(nativeProps);
    }
  },

  _onBlur: function (event: Event) {
    // [TODO(android ISS)
    // Set the focused TextInput field info in TextInputState.
    // Delaying this to onBlur native event ensures that -
    // 1. The state is updated only after the native code completes clearing focus on the view
    // 2. In case the focus is moving from one TextInput(A) to another TextInput(B), the state of
    //    A needs to be updated (blurred) before info about B is updated in TestInputState.
    TextInputState.clearFocusedTextInput(ReactNative.findNodeHandle(this._inputRef)); // ]TODO(android ISS)
    if (this.props.onBlur) {
      this.props.onBlur(event);
    }
  },

  _onTextInput: function (event: Event) {
    this.props.onTextInput && this.props.onTextInput(event);
  },

  _onScroll: function (event: Event) {
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