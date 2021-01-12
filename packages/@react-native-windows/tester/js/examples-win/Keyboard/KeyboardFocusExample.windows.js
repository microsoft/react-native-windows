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
// TextInput2 is used to verify supportKeyboard + focus
const TextInput2 = react_native_windows_1.supportKeyboard(react_native_1.TextInput);
const styles = react_native_1.StyleSheet.create({
    border: {
        borderStyle: 'dotted',
        borderColor: 'black',
    },
    keyComponentRoot: {
        borderWidth: 2,
        flexDirection: 'column',
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
const pickerRef = React.createRef();
const viewWindowsRef = React.createRef();
const textInputRef = React.createRef();
const textInputRef2 = React.createRef();
const touchableHighlightRef = React.createRef();
const touchableOpacityRef = React.createRef();
const touchableWithoutFeedbackRef = React.createRef();
class KeyboardFocusExample extends React.Component {
    constructor(props) {
        super(props);
        this._textInputKeyDown = (ev) => {
            this.setState({ keyOnKeyDown: ev.nativeEvent.key });
        };
        this._selectionChanged = (selected) => {
            switch (selected) {
                case 'View':
                    viewWindowsRef.current && viewWindowsRef.current.focus();
                    break;
                case 'Picker':
                    // pickerRef.current && pickerRef.current.focus();
                    break;
                case 'TextInput':
                    textInputRef.current && textInputRef.current.focus();
                    if (textInputRef.current) {
                        textInputRef.current.focus();
                        setTimeout(() => {
                            textInputRef2.current && textInputRef2.current.blur();
                        }, 10000);
                    }
                    break;
                case 'TextInput2':
                    if (textInputRef2.current) {
                        textInputRef2.current.focus();
                        setTimeout(() => {
                            textInputRef2.current && textInputRef2.current.blur();
                        }, 10000);
                    }
                    break;
                case 'TouchableHighlight':
                    touchableHighlightRef.current && touchableHighlightRef.current.focus();
                    break;
                case 'TouchableOpacity':
                    touchableOpacityRef.current && touchableOpacityRef.current.focus();
                    break;
                case 'TouchableWithoutFeedback':
                    // TouchableWithoutFeedback doesn't have a focus method, since it doesn't have NativeMethodsMixin applied
                    break;
            }
        };
        this._touchableHighlightFocus = () => {
            this.setState({ focusMessageHighlight: 'TouchableHighlight onFocus' });
        };
        this._touchableHighlightBlur = () => {
            this.setState({ focusMessageHighlight: 'TouchableHighlight onBlur' });
        };
        this._touchableOpacityFocus = () => {
            this.setState({ focusMessageOpacity: 'TouchableOpacity onFocus' });
        };
        this._touchableOpacityBlur = () => {
            this.setState({ focusMessageOpacity: 'TouchableOpacity onBlur' });
        };
        this._touchableWithoutFeedbackFocus = () => {
            this.setState({
                focusMessageWithoutFeedback: 'TouchableWithoutFeedback onFocus',
            });
        };
        this._touchableWithoutFeedbackBlur = () => {
            this.setState({
                focusMessageWithoutFeedback: 'TouchableWithoutFeedback onBlur',
            });
        };
        this.state = {
            selected: '',
            keyOnKeyDown: 'unknown',
            focusMessageHighlight: '',
            focusMessageOpacity: '',
            focusMessageWithoutFeedback: '',
        };
    }
    render() {
        const pickerItems = [
            'View',
            'Picker',
            'TextInput',
            'TextInput2',
            'TouchableHighlight',
            'TouchableOpacity',
        ];
        return (React.createElement(react_native_1.View, { style: styles.keyComponentRoot },
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.Text, null, "Please select a item to set focus")),
            React.createElement(react_native_windows_1.Picker, { selectedValue: this.state.selected, onValueChange: this._selectionChanged }, pickerItems.map(item => (React.createElement(react_native_windows_1.Picker.Item, { key: item, value: item, label: item })))),
            React.createElement(react_native_windows_1.ViewWindows, { style: styles.keyEnterVisualizer, focusable: true, ref: viewWindowsRef },
                React.createElement(react_native_1.Text, null, "View accept focus")),
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_windows_1.Picker, { ref: pickerRef },
                    React.createElement(react_native_windows_1.Picker.Item, { label: "Picker accept focus" }))),
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.Text, null, "Test Purpose: focus on TextInput, then timeout and blur on TextInput2, TextInput still keep focus"),
                React.createElement(react_native_1.TextInput, { placeholder: "TextInput accept focus", style: styles.textInput, ref: textInputRef })),
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.Text, null, "Test Purpose: focus on TextInput2, then timeout and blur on TextInput2, TextInput2 lose focus"),
                React.createElement(TextInput2, { onKeyDown: this._textInputKeyDown, placeholder: "TextInput accept focus", style: styles.textInput, ref: textInputRef2 }),
                React.createElement(react_native_1.Text, null,
                    "Key ",
                    this.state.keyOnKeyDown)),
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.TouchableHighlight, { ref: touchableHighlightRef, onFocus: this._touchableHighlightFocus, onBlur: this._touchableHighlightBlur },
                    React.createElement(react_native_1.View, { style: { width: 50, height: 50, backgroundColor: 'blue' } })),
                React.createElement(react_native_1.Text, null,
                    "Last focus event for TouchableHighlight:",
                    ' ',
                    this.state.focusMessageHighlight),
                React.createElement(react_native_1.TouchableOpacity, { ref: touchableOpacityRef, onFocus: this._touchableOpacityFocus, onBlur: this._touchableOpacityBlur },
                    React.createElement(react_native_1.View, { style: { width: 50, height: 50, backgroundColor: 'yellow' } })),
                React.createElement(react_native_1.Text, null,
                    "Last focus event for TouchableOpacity:",
                    ' ',
                    this.state.focusMessageOpacity),
                React.createElement(react_native_1.TouchableWithoutFeedback, { ref: touchableWithoutFeedbackRef, onFocus: this._touchableWithoutFeedbackFocus, onBlur: this._touchableWithoutFeedbackBlur },
                    React.createElement(react_native_1.View, { style: { width: 50, height: 50, backgroundColor: 'green' } })),
                React.createElement(react_native_1.Text, null,
                    "Last focus event for TouchableWithoutFeedback:",
                    ' ',
                    this.state.focusMessageWithoutFeedback))));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'Keyboard Focus Example';
exports.category = 'Basic';
exports.description = 'Demo of keyboard focus.';
exports.examples = [
    {
        title: 'Keyboard Focus example',
        render() {
            return React.createElement(KeyboardFocusExample, null);
        },
    },
];
//# sourceMappingURL=KeyboardFocusExample.windows.js.map