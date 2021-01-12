"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.category = exports.title = exports.displayName = void 0;
const React = require("react");
const react_native_1 = require("react-native");
const react_native_windows_1 = require("react-native-windows");
const ViewWindows = react_native_windows_1.supportKeyboard(react_native_1.View);
const styles = react_native_1.StyleSheet.create({
    border: {
        borderStyle: 'dotted',
        borderColor: 'black',
    },
    keyComponentRoot: {
        borderWidth: 2,
        flexDirection: 'row',
        marginVertical: 5,
        backgroundColor: 'whitesmoke',
        justifyContent: 'space-around',
    },
    keyEnterVisualizer: {
        margin: 5,
        alignItems: 'center',
        minWidth: 100,
        minHeight: 30,
    },
    textInput: {
        height: 32,
        width: 100,
    },
    blackbox: { height: 30, width: 30, borderColor: 'black', borderWidth: 3 },
});
const handledNativeKeyboardEvents = [
    { code: 'KeyA', handledEventPhase: react_native_windows_1.HandledEventPhase.Capturing },
    { code: 'KeyB' },
    { code: 'Digit1', handledEventPhase: react_native_windows_1.HandledEventPhase.Bubbling },
    { code: 'Tab', handledEventPhase: react_native_windows_1.HandledEventPhase.Capturing },
];
class ViewWindowsKeyboardExample extends React.Component {
    constructor(props) {
        super(props);
        this._onKeyUp = (ev) => {
            this.setState({
                lastKeyUp: ev.nativeEvent.key,
                lastKeyDown: null,
                lastKeyUpCode: ev.nativeEvent.code,
                lastKeyDownCode: null,
            });
        };
        this._onKeyDown = (ev) => {
            this.setState({
                lastKeyDown: ev.nativeEvent.key,
                lastKeyUp: null,
                lastKeyDownCode: ev.nativeEvent.code,
                lastKeyUpCode: null,
            });
        };
        this._onKeyUpCapture = (ev) => {
            this.setState({
                lastKeyUpCapture: ev.nativeEvent.key,
                lastKeyDownCapture: null,
                lastKeyUpCaptureCode: ev.nativeEvent.code,
                lastKeyDownCaptureCode: null,
            });
        };
        this._onKeyDownCapture = (ev) => {
            this.setState({
                lastKeyDownCapture: ev.nativeEvent.key,
                lastKeyUpCapture: null,
                lastKeyDownCaptureCode: ev.nativeEvent.code,
                lastKeyUpCaptureCode: null,
            });
        };
        this.state = {
            lastKeyDown: null,
            lastKeyUp: null,
            lastKeyDownCapture: null,
            lastKeyUpCapture: null,
            lastKeyDownCode: null,
            lastKeyUpCode: null,
            lastKeyDownCaptureCode: null,
            lastKeyUpCaptureCode: null,
        };
    }
    render() {
        return (React.createElement(ViewWindows, null,
            React.createElement(ViewWindows, { style: styles.keyComponentRoot, onKeyDownCapture: this._onKeyDownCapture, onKeyUpCapture: this._onKeyUpCapture, onKeyUp: this._onKeyUp, onKeyDown: this._onKeyDown, keyDownEvents: handledNativeKeyboardEvents, keyUpEvents: handledNativeKeyboardEvents },
                React.createElement(ViewWindows, { style: styles.keyEnterVisualizer },
                    React.createElement(react_native_1.Text, null, "OnKeyDown Key and Code"),
                    React.createElement(react_native_1.Text, null, this.state.lastKeyDown !== null ? this.state.lastKeyDown : ' '),
                    React.createElement(react_native_1.Text, null, this.state.lastKeyDownCode !== null
                        ? this.state.lastKeyDownCode
                        : ' '),
                    React.createElement(react_native_1.Text, null, "OnKeyDownCapture Key and Code"),
                    React.createElement(react_native_1.Text, null, this.state.lastKeyDownCapture !== null
                        ? this.state.lastKeyDownCapture
                        : ' '),
                    React.createElement(react_native_1.Text, null, this.state.lastKeyDownCaptureCode !== null
                        ? this.state.lastKeyDownCaptureCode
                        : ' ')),
                React.createElement(ViewWindows, { style: styles.keyEnterVisualizer },
                    React.createElement(react_native_1.Text, null, "OnKeyUp Key and Code"),
                    React.createElement(react_native_1.Text, null, this.state.lastKeyUp !== null ? this.state.lastKeyUp : ' '),
                    React.createElement(react_native_1.Text, null, this.state.lastKeyUpCode !== null
                        ? this.state.lastKeyUpCode
                        : ' '),
                    React.createElement(react_native_1.Text, null, "OnKeyUpCapture Key and Code"),
                    React.createElement(react_native_1.Text, null, this.state.lastKeyUpCapture !== null
                        ? this.state.lastKeyUpCapture
                        : ' '),
                    React.createElement(react_native_1.Text, null, this.state.lastKeyUpCaptureCode !== null
                        ? this.state.lastKeyUpCaptureCode
                        : ' ')),
                React.createElement(ViewWindows, { style: styles.keyEnterVisualizer },
                    React.createElement(react_native_1.TextInput, { placeholder: "I got focus", style: styles.textInput })))));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'Keyboard extension Example';
exports.category = 'Basic';
exports.description = 'Demo of keyboard properties.';
exports.examples = [
    {
        title: 'Keyboard extension example',
        render() {
            return React.createElement(ViewWindowsKeyboardExample, null);
        },
    },
];
//# sourceMappingURL=KeyboardExtensionExample.windows.js.map