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
var KeyboardExtProps_1 = require("../../../../Libraries/Components/Keyboard/KeyboardExtProps");
var KeyboardExt_1 = require("../../../../Libraries/Components/Keyboard/KeyboardExt");
var ViewWindows = KeyboardExt_1.supportKeyboard(react_native_1.View);
var styles = react_native_1.StyleSheet.create({
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
var handledNativeKeyboardEvents = [
    { code: 'KeyA', handledEventPhase: KeyboardExtProps_1.HandledEventPhase.Capturing },
    { code: 'KeyB' },
    { code: 'Digit1', handledEventPhase: KeyboardExtProps_1.HandledEventPhase.Bubbling },
    { code: 'Tab', handledEventPhase: KeyboardExtProps_1.HandledEventPhase.Capturing },
];
var ViewWindowsKeyboardExample = /** @class */ (function (_super) {
    __extends(ViewWindowsKeyboardExample, _super);
    function ViewWindowsKeyboardExample(props) {
        var _this = _super.call(this, props) || this;
        _this._onKeyUp = function (ev) {
            _this.setState({
                lastKeyUp: ev.nativeEvent.key,
                lastKeyDown: null,
                lastKeyUpCode: ev.nativeEvent.code,
                lastKeyDownCode: null,
            });
        };
        _this._onKeyDown = function (ev) {
            _this.setState({
                lastKeyDown: ev.nativeEvent.key,
                lastKeyUp: null,
                lastKeyDownCode: ev.nativeEvent.code,
                lastKeyUpCode: null,
            });
        };
        _this._onKeyUpCapture = function (ev) {
            _this.setState({
                lastKeyUpCapture: ev.nativeEvent.key,
                lastKeyDownCapture: null,
                lastKeyUpCaptureCode: ev.nativeEvent.code,
                lastKeyDownCaptureCode: null,
            });
        };
        _this._onKeyDownCapture = function (ev) {
            _this.setState({
                lastKeyDownCapture: ev.nativeEvent.key,
                lastKeyUpCapture: null,
                lastKeyDownCaptureCode: ev.nativeEvent.code,
                lastKeyUpCaptureCode: null,
            });
        };
        _this.state = {
            lastKeyDown: null,
            lastKeyUp: null,
            lastKeyDownCapture: null,
            lastKeyUpCapture: null,
            lastKeyDownCode: null,
            lastKeyUpCode: null,
            lastKeyDownCaptureCode: null,
            lastKeyUpCaptureCode: null,
        };
        return _this;
    }
    ViewWindowsKeyboardExample.prototype.render = function () {
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
    };
    return ViewWindowsKeyboardExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = 'Keyboard extension Example';
exports.description = 'Demo of keyboard properties.';
exports.examples = [
    {
        title: 'Keyboard extension example',
        render: function () {
            return React.createElement(ViewWindowsKeyboardExample, null);
        },
    },
];
//# sourceMappingURL=KeyboardExtensionExample.windows.js.map