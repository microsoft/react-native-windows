/**
 * @providesModule TextInput
 */
'use strict';
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
/**
 * This is a forked and slightly modified version of React Native's TextInput.
 * The fork is necessary as platform checks in the base implementation made the
 * control unusable on win32. In addition to cleaning up some of the code, this
 * fork also uses Typescript rather than Flow for type safety.
 *
 * More general documentation on this control can be found at
 * https://facebook.github.io/react-native/docs/textinput.html
 *
 * The original implementation can be found at
 * https://github.com/facebook/react-native/blob/1013a010492a7bef5ff58073a088ac924a986e9e/Libraries/Components/TextInput/TextInput.js
 *
 * This control does not support the full React Native TextInput interface yet.
 * Most of the work necessary to make that happen needs to happen on the native side.
 * Future work on the JS side may include:
 * 1. Expanded typings for some of the events
 * 2. Additional work to manage selection
 * 3. Any base/default styling work
 */
const react_1 = __importDefault(require("react"));
const react_native_1 = require("react-native");
const TextAncestor = require('../../Text/TextAncestor');
const TextInputState = require('./TextInputState');
// RCTTextInput is the native component that win32 understands
const RCTTextInput = react_native_1.requireNativeComponent('RCTTextInput');
// Adding typings on ViewManagers is problematic as available functionality is not known until
// registration at runtime and would require native and js to always be in sync.
const TextInputViewManager = react_native_1.NativeModules.UIManager.getViewManagerConfig('RCTTextInput');
class TextInput extends react_1.default.Component {
    constructor(props) {
        super(props);
        this._eventCount = 0;
        /**
         * Focuses the TextInput
         */
        this.focus = () => {
            TextInputState.setFocusedTextInput(this);
            react_native_1.NativeModules.UIManager.
                dispatchViewManagerCommand(react_native_1.findNodeHandle(this), TextInputViewManager.Commands.focus, null);
        };
        /**
         * Blurs the TextInput
         */
        this.blur = () => {
            TextInputState.blurTextInput(this);
            react_native_1.NativeModules.UIManager.
                dispatchViewManagerCommand(react_native_1.findNodeHandle(this), TextInputViewManager.Commands.blur, null);
        };
        /**
         * Use clear for programmatic clearing of the text
         */
        this.clear = () => {
            this.setNativeText('');
        };
        this.setEventCount = () => {
            react_native_1.NativeModules.UIManager.
                dispatchViewManagerCommand(react_native_1.findNodeHandle(this), TextInputViewManager.Commands.setEventCount, { eventCount: this._eventCount });
        };
        this.setNativeText = (val) => {
            if (this._lastNativeText !== val) {
                react_native_1.NativeModules.UIManager.
                    dispatchViewManagerCommand(react_native_1.findNodeHandle(this), TextInputViewManager.Commands.setNativeText, { text: val });
            }
        };
        this._getText = () => {
            return this.props.value || (this.props.defaultValue || null);
        };
        this._onChange = (e) => {
            const text = e.nativeEvent.text;
            this._eventCount = e.nativeEvent.eventCount;
            this.setEventCount();
            this.props.onChange && this.props.onChange(e);
            this.props.onChangeText && this.props.onChangeText(text);
            this._lastNativeText = text;
            this.forceUpdate();
            return;
        };
        this._onFocus = (e) => {
            this.focus();
            this.props.onFocus && this.props.onFocus(e);
        };
        this._onBlur = (e) => {
            this.props.onBlur && this.props.onBlur(e);
        };
        this._inputRef = react_1.default.createRef();
    }
    /**
     * On mount TextInput needs to register itself with TextInputState
     * and conditionally request an animation frame for focus.
     */
    componentDidMount() {
        this._lastNativeText = this.props.value;
        TextInputState.registerInput(this);
        if (this.props.autoFocus) {
            this._rafID = requestAnimationFrame(this.focus);
        }
    }
    /**
     * This is an unfortunate consequence of having controlled TextInputs.
     * Tree diffing reconciliation will not always send down text values
     * This sets text explicitly.
     */
    componentDidUpdate() {
        if ((this._lastNativeText !== this._getText())) {
            this._getText() && this.setNativeText(this._getText());
        }
        return;
    }
    /**
     * Pre-unmoun the TextInput should blur, unregister and clean up
     * the animation frame for focus (edge cases)
     */
    componentWillUnmount() {
        // blur
        if (this.isFocused()) {
            this.blur();
        }
        // unregister
        TextInputState.unregisterInput(this);
        // cancel animationFrame
        if (this._rafID !== null) {
            cancelAnimationFrame(this._rafID);
        }
        if (this._rafID) {
            return;
        }
        return;
    }
    render() {
        const props = Object.assign({}, this.props);
        // By default fontScaling is turned on.
        // win32 ignores this prop for the time being
        props.allowFontScaling = (this.props.allowFontScaling === null
            || this.props.allowFontScaling === undefined)
            ? true : this.props.allowFontScaling;
        return (react_1.default.createElement(TextAncestor.Provider, { value: true },
            react_1.default.createElement(RCTTextInput, Object.assign({}, props, { ref: this._inputRef, text: this._getText(), onFocus: this._onFocus, onBlur: this._onBlur, onChange: this._onChange }))));
    }
    /**
     * Returns true if the TextInput is focused
     */
    isFocused() {
        return TextInputState.currentlyFocusedInput() === this;
    }
}
module.exports = TextInput;
//# sourceMappingURL=TextInput.win32.js.map