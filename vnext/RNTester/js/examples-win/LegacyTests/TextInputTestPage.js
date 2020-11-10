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
exports.examples = exports.description = exports.title = exports.displayName = exports.TextInputTestPage = void 0;
var React = require("react");
var react_native_1 = require("react-native");
var Consts_1 = require("./Consts");
var TextInputTestPage = /** @class */ (function (_super) {
    __extends(TextInputTestPage, _super);
    function TextInputTestPage() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            log: '<Log Start>',
        };
        _this.updateText = function (text) {
            _this.setState(function (state) {
                return {
                    log: text + '\n' + state.log,
                };
            });
        };
        return _this;
    }
    TextInputTestPage.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.TextInput, { style: { height: 32 }, testID: Consts_1.TEXTINPUT_ON_TEXTINPUT, placeholder: "Enter text to see events", onFocus: function () { return _this.updateText('onFocus'); }, onBlur: function () { return _this.updateText('onBlur'); }, onChange: function (event) {
                    return _this.updateText('onChange text: ' + event.nativeEvent.text);
                }, onEndEditing: function (event) {
                    return _this.updateText('onEndEditing text: ' + event.nativeEvent.text);
                }, onSubmitEditing: function (event) {
                    return _this.updateText('onSubmitEditing text: ' + event.nativeEvent.text);
                }, onSelectionChange: function (event) {
                    _this.updateText('onSelectionChange range: ' +
                        event.nativeEvent.selection.start +
                        ',' +
                        event.nativeEvent.selection.end);
                }, onKeyPress: function (event) {
                    _this.updateText('onKeyPress key: ' + event.nativeEvent.key);
                } }),
            React.createElement(react_native_1.TextInput, { testID: Consts_1.ML_TEXTINPUT_ON_TEXTINPUT, style: { height: 80 }, placeholder: "MultiLine", multiline: true }),
            React.createElement(react_native_1.TextInput, { testID: Consts_1.CAP_TEXTINPUT_ON_TEXTINPUT, style: { height: 80 }, placeholder: "autoCapitalize", autoCapitalize: "characters" }),
            React.createElement(react_native_1.Text, { testID: Consts_1.CURTEXT_ON_TEXTINPUT }, this.state.log)));
    };
    return TextInputTestPage;
}(React.Component));
exports.TextInputTestPage = TextInputTestPage;
exports.displayName = function (_undefined) { };
exports.title = '<LegacyTextInputTest>';
exports.description = 'Legacy e2e test for TextInput';
exports.examples = [
    {
        render: function () {
            return React.createElement(TextInputTestPage, null);
        },
    },
];
//# sourceMappingURL=TextInputTestPage.js.map