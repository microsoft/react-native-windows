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
            function (d, b) { for (var p in b) if (Object.prototype.hasOwnProperty.call(b, p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.DisplayNoneExample = void 0;
var React = require("react");
var react_native_1 = require("react-native");
var RNTesterBlock = require('../../components/RNTesterBlock');
var RNTesterPage = require('../../components/RNTesterPage');
var DisplayNoneExample = /** @class */ (function (_super) {
    __extends(DisplayNoneExample, _super);
    function DisplayNoneExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            displayNone: false,
            displayNone2: false,
            textState: 'TextInput1...',
            textState2: 'TextInput2...',
        };
        _this._handlePress = function () {
            _this.setState({ displayNone: !_this.state.displayNone });
        };
        _this._handlePress2 = function () {
            _this.setState({ displayNone2: !_this.state.displayNone2 });
        };
        _this._handleChangeText = function (text) {
            _this.setState({ textState: text });
        };
        _this._handleChangeText2 = function (text) {
            _this.setState({ textState2: text });
        };
        return _this;
    }
    DisplayNoneExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(RNTesterPage, null,
            React.createElement(RNTesterBlock, { title: "Display:none style with TextInput" },
                React.createElement(react_native_1.View, null,
                    React.createElement(react_native_1.View, { testID: "view-component-switch-view" },
                        React.createElement(react_native_1.Text, null, "toggle display:none TextInput1"),
                        React.createElement(react_native_1.Switch, { onValueChange: this._handlePress, value: this.state.displayNone }),
                        React.createElement(react_native_1.Text, null, "toggle display:none TextInput2"),
                        React.createElement(react_native_1.Switch, { onValueChange: this._handlePress2, value: this.state.displayNone2, testID: "toggle-display:none" })),
                    React.createElement(react_native_1.View, { style: { display: this.state.displayNone ? 'none' : 'flex' } },
                        React.createElement(react_native_1.TextInput, { style: { height: 40, borderColor: 'gray', borderWidth: 1 }, onChangeText: function (text) { return _this._handleChangeText(text); }, value: this.state.textState })),
                    React.createElement(react_native_1.View, { style: { display: this.state.displayNone2 ? 'none' : 'flex' } },
                        React.createElement(react_native_1.TextInput, { style: { height: 40, borderColor: 'gray', borderWidth: 1 }, onChangeText: function (text) { return _this._handleChangeText2(text); }, value: this.state.textState2 }))))));
    };
    return DisplayNoneExample;
}(React.Component));
exports.DisplayNoneExample = DisplayNoneExample;
exports.displayName = function (_undefined) { };
exports.title = 'Display:none Style';
exports.description = 'Style prop which will collapse the element in XAML tree.';
exports.examples = [
    {
        render: function () {
            return React.createElement(DisplayNoneExample, null);
        },
    },
];
//# sourceMappingURL=DisplayNoneExample.windows.js.map