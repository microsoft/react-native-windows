"use strict";
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
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
/* tslint:disable */
var React = require("react");
var react_native_1 = require("react-native");
var index_uwp_1 = require("../../src/index.uwp");
var PopupExample = /** @class */ (function (_super) {
    __extends(PopupExample, _super);
    function PopupExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = {
            isFlyoutVisible: false,
            buttonTitle: 'Open Flyout',
            isLightDismissEnabled: false
        };
        _this._setRef = function (textInput) {
            _this._textInput = textInput;
        };
        _this._onPress = function () {
            _this.setState({ buttonTitle: 'Close Flyout', isFlyoutVisible: true });
        };
        _this._onPopupButtonPressed = function () {
            _this.setState({ buttonTitle: 'Open Flyout', isFlyoutVisible: false });
        };
        _this._onPopupDismissed = function (isOpen) {
            _this.setState({ buttonTitle: 'Open Flyout', isFlyoutVisible: isOpen });
        };
        _this._textInput = React.createRef();
        return _this;
    }
    PopupExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { style: { width: 250 } }, "The following tests popup Anchor"),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10, width: 300, height: 32 } }, "Text Input to Anchor popup to: "),
                React.createElement(react_native_1.TextInput, { style: { height: 32, width: 300 }, ref: this._setRef })),
            React.createElement(react_native_1.View, { style: { justifyContent: 'center', padding: 50 } },
                React.createElement(react_native_1.Button, { onPress: this._onPress, title: this.state.buttonTitle })),
            this.state.isFlyoutVisible && (React.createElement(index_uwp_1.Popup, { isOpen: this.state.isFlyoutVisible, isLightDismissEnabled: this.state.isLightDismissEnabled, onDismiss: this._onPopupDismissed, target: this._textInput, horizontalOffset: 10, verticalOffset: 10 },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightgray', width: 200, height: 300 } },
                    React.createElement(react_native_1.Text, { style: { justifyContent: 'center', paddingTop: 10, paddingBottom: 30 } }, "This is a flyout"),
                    React.createElement(react_native_1.Button, { onPress: this._onPopupButtonPressed, title: "Close" }))))));
    };
    return PopupExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = '<Popup>';
exports.description = 'Displays content on top of existing content, within the bounds of the application window.';
exports.examples = [
    {
        title: 'Popup Anchor to text input',
        render: function () {
            return React.createElement(PopupExample, null);
        },
    }
];
//# sourceMappingURL=PopupExample.uwp.js.map