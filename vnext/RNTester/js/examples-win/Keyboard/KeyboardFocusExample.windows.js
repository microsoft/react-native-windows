"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = void 0;
var React = require("react");
var react_native_1 = require("react-native");
var PickerWindows_1 = require("../../../../Libraries/Components/Picker/PickerWindows");
var KeyboardExt_1 = require("../../../../Libraries/Components/Keyboard/KeyboardExt");
var ViewWindows_1 = require("../../../../Libraries/Components/View/ViewWindows");
// TextInput2 is used to verify supportKeyboard + focus
var TextInput2 = KeyboardExt_1.supportKeyboard(react_native_1.TextInput);
var styles = react_native_1.StyleSheet.create({
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
var pickerRef = React.createRef();
var viewWindowsRef = React.createRef();
var textInputRef = React.createRef();
// tslint:disable-next-line
var textInputRef2 = React.createRef();
var touchableHighlightRef = React.createRef();
var touchableOpacityRef = React.createRef();
var touchableWithoutFeedbackRef = React.createRef();
var KeyboardFocusExample = /** @class */ (function (_super) {
    __extends(KeyboardFocusExample, _super);
    function KeyboardFocusExample(props) {
        var _this = _super.call(this, props) || this;
        _this._textInputKeyDown = function (ev) {
            _this.setState({ keyOnKeyDown: ev.nativeEvent.key });
        };
        _this._selectionChanged = function (selected) {
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
                        setTimeout(function () {
                            textInputRef2.current && textInputRef2.current.blur();
                        }, 10000);
                    }
                    break;
                case 'TextInput2':
                    if (textInputRef2.current) {
                        textInputRef2.current.focus();
                        setTimeout(function () {
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
        _this._touchableHighlightFocus = function () {
            _this.setState({ focusMessageHighlight: 'TouchableHighlight onFocus' });
        };
        _this._touchableHighlightBlur = function () {
            _this.setState({ focusMessageHighlight: 'TouchableHighlight onBlur' });
        };
        _this._touchableOpacityFocus = function () {
            _this.setState({ focusMessageOpacity: 'TouchableOpacity onFocus' });
        };
        _this._touchableOpacityBlur = function () {
            _this.setState({ focusMessageOpacity: 'TouchableOpacity onBlur' });
        };
        _this._touchableWithoutFeedbackFocus = function () {
            _this.setState({
                focusMessageWithoutFeedback: 'TouchableWithoutFeedback onFocus',
            });
        };
        _this._touchableWithoutFeedbackBlur = function () {
            _this.setState({
                focusMessageWithoutFeedback: 'TouchableWithoutFeedback onBlur',
            });
        };
        _this.state = {
            selected: '',
            keyOnKeyDown: 'unknown',
            focusMessageHighlight: '',
            focusMessageOpacity: '',
            focusMessageWithoutFeedback: '',
        };
        return _this;
    }
    KeyboardFocusExample.prototype.render = function () {
        var pickerItems = [
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
            React.createElement(PickerWindows_1.Picker, { selectedValue: this.state.selected, onValueChange: this._selectionChanged }, pickerItems.map(function (item) { return (React.createElement(PickerWindows_1.Picker.Item, { key: item, value: item, label: item })); })),
            React.createElement(ViewWindows_1.ViewWindows, { style: styles.keyEnterVisualizer, focusable: true, ref: viewWindowsRef },
                React.createElement(react_native_1.Text, null, "View accept focus")),
            React.createElement(react_native_1.View, null,
                React.createElement(PickerWindows_1.Picker, { ref: pickerRef },
                    React.createElement(PickerWindows_1.Picker.Item, { label: "Picker accept focus" }))),
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
    };
    return KeyboardFocusExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = 'Keyboard Focus Example';
exports.description = 'Demo of keyboard focus.';
exports.examples = [
    {
        title: 'Keyboard Focus example',
        render: function () {
            return React.createElement(KeyboardFocusExample, null);
        },
    },
];
//# sourceMappingURL=KeyboardFocusExample.windows.js.map