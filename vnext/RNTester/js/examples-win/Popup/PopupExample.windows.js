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
var Popup_1 = require("../../../../Libraries/Components/Popup/Popup");
var AnchoredPopupExample = /** @class */ (function (_super) {
    __extends(AnchoredPopupExample, _super);
    function AnchoredPopupExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = {
            showPopup: false,
            buttonTitle: 'Open Popup',
        };
        _this._togglePopup = function () {
            _this.setState(function (state) { return ({
                buttonTitle: state.showPopup ? 'Open Popup' : 'Close Popup',
                showPopup: !state.showPopup,
            }); });
        };
        _this._onPopupDismissed = function () {
            _this.setState({ buttonTitle: 'Open Popup', showPopup: false });
        };
        _this._textInput = React.createRef();
        return _this;
    }
    AnchoredPopupExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { style: { width: 250 } }, "The following tests popup Anchor"),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10, width: 300, height: 32 } },
                    "Text Input to Anchor popup to:",
                    ' '),
                React.createElement(react_native_1.TextInput, { style: { height: 32, width: 300 }, ref: this._textInput })),
            React.createElement(react_native_1.View, { style: { justifyContent: 'center', padding: 50 } },
                React.createElement(react_native_1.Button, { onPress: this._togglePopup, title: this.state.buttonTitle })),
            this.state.showPopup && (React.createElement(Popup_1.Popup, { isOpen: this.state.showPopup, onDismiss: this._onPopupDismissed, target: this._textInput.current, isLightDismissEnabled: true, horizontalOffset: 10, verticalOffset: 10 },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightgray', width: 200, height: 300 } },
                    React.createElement(react_native_1.Text, { style: {
                            justifyContent: 'center',
                            paddingTop: 10,
                            paddingBottom: 30,
                        } }, "This is a flyout"),
                    React.createElement(react_native_1.Button, { onPress: this._togglePopup, title: "Close" }))))));
    };
    return AnchoredPopupExample;
}(React.Component));
var PopupPlacementExample = /** @class */ (function (_super) {
    __extends(PopupPlacementExample, _super);
    function PopupPlacementExample(props) {
        var _this = _super.call(this, props) || this;
        _this._togglePopup = function () {
            _this.setState(function (state) { return ({
                showPopup: !state.showPopup,
            }); });
        };
        _this._onPopupDismissed = function () {
            _this.setState({ showPopup: false });
        };
        _this._noop = function () {
            return;
        };
        _this.state = {
            showPopup: false,
        };
        return _this;
    }
    PopupPlacementExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, { style: { width: 500, height: 500 } },
            React.createElement(react_native_1.Button, { onPress: this._togglePopup, title: 'Toggle popup' }),
            this.state.showPopup && (React.createElement(Popup_1.Popup, { style: { position: 'absolute', left: 0, right: 0, top: 0, bottom: 0 }, isOpen: this.state.showPopup, onDismiss: this._onPopupDismissed, isLightDismissEnabled: false },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightgray', flex: 1 } },
                    React.createElement(react_native_1.Text, null, "This is a popup"),
                    React.createElement(react_native_1.Button, { onPress: this._togglePopup, title: "Toggle popup" }),
                    React.createElement(react_native_1.Button, { onPress: this._noop, title: "This is a button" }))))));
    };
    return PopupPlacementExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = '<Popup>';
exports.description = 'Displays content on top of existing content, within the bounds of the application window.';
exports.examples = [
    {
        title: 'Popup Anchor to text input w/ light dismiss',
        render: function () {
            return React.createElement(AnchoredPopupExample, null);
        },
    },
    {
        title: 'Popup centered on screen',
        render: function () {
            return React.createElement(PopupPlacementExample, null);
        },
    },
];
//# sourceMappingURL=PopupExample.windows.js.map