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
var Flyout_1 = require("../../../../Libraries/Components/Flyout/Flyout");
var PickerWindows_1 = require("../../../../Libraries/Components/Picker/PickerWindows");
var Popup_1 = require("../../../../Libraries/Components/Popup/Popup");
var placementValues = [
    'top',
    'bottom',
    'left',
    'right',
    'full',
    'top-edge-aligned-left',
    'top-edge-aligned-right',
    'bottom-edge-aligned-left',
    'bottom-edge-aligned-right',
    'left-edge-aligned-top',
    'right-edge-aligned-top',
    'left-edge-aligned-bottom',
    'right-edge-aligned-bottom',
];
var FlyoutExample = /** @class */ (function (_super) {
    __extends(FlyoutExample, _super);
    function FlyoutExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = {
            isFlyoutVisible: false,
            isFlyoutTwoVisible: false,
            isPopupVisible: false,
            buttonTitle: 'Open Flyout',
            isLightDismissEnabled: true,
            isOverlayEnabled: false,
            popupSwitchState: true,
            placementOptions: 'top',
        };
        _this._setRef = function (textInput) {
            _this._anchor = textInput;
        };
        _this._onPress = function () {
            _this.setState({ buttonTitle: 'Close Flyout', isFlyoutVisible: true });
        };
        _this._onFlyoutButtonPressed = function () {
            _this.setState({ buttonTitle: 'Open Flyout', isFlyoutVisible: false });
        };
        _this._onFlyoutDismissed = function (_isOpen) {
            _this.setState({ isFlyoutVisible: false });
            _this.setState({ isFlyoutTwoVisible: false });
            _this.setState({ isPopupVisible: false });
            _this.setState({ buttonTitle: 'Open Flyout' });
        };
        _this._setRefTwo = function (element) {
            _this._anchorTwo = element;
        };
        _this._onPressTwo = function () {
            _this.setState({ isFlyoutTwoVisible: true });
        };
        _this._onFlyoutTwoDismissed = function (_isOpen) {
            _this.setState({ isFlyoutTwoVisible: false });
        };
        _this._anchor = React.createRef();
        _this._anchorTwo = React.createRef();
        return _this;
    }
    FlyoutExample.prototype.render = function () {
        var _this = this;
        var lorumIpsum = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed dapibus felis eget augue condimentum suscipit. Suspendisse hendrerit, libero aliquet malesuada tempor, urna nibh consectetur tellus, vitae efficitur quam erat non mi. Maecenas vitae eros sit amet quam vestibulum porta sed sit amet tellus. Fusce quis lectus congue, fringilla arcu id, luctus urna. Cras sagittis ornare mauris sit amet dictum. Vestibulum feugiat laoreet fringilla. Vivamus ac diam vehicula felis venenatis sagittis vitae ultrices elit. Curabitur libero augue, laoreet quis orci vitae, congue euismod massa. Aenean nec odio sed urna vehicula fermentum non a magna. Quisque ut commodo neque, eget eleifend odio. Sed sit amet lacinia sem. Suspendisse in metus in purus scelerisque vestibulum. Nam metus dui, efficitur nec metus non, tincidunt pharetra sapien. Praesent id convallis metus, ut malesuada arcu. Quisque quam libero, pharetra eu tellus ac, aliquam fringilla erat. Quisque tempus in lorem ac suscipit.';
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.View, { style: { flexDirection: 'row', paddingTop: 20 } },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "Placement Options: "),
                React.createElement(PickerWindows_1.Picker, { style: { width: 200, height: 35 }, selectedValue: this.state.placementOptions, onValueChange: function (value) { return _this.setState({ placementOptions: value }); } }, placementValues.map(function (item) { return (React.createElement(PickerWindows_1.Picker.Item, { key: item, label: item, value: item })); }))),
            React.createElement(react_native_1.View, { style: { justifyContent: 'center', padding: 20, width: 200 } },
                React.createElement(react_native_1.Button, { onPress: this._onPress, title: this.state.buttonTitle })),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row', paddingTop: 200 } },
                React.createElement(react_native_1.Text, { style: { padding: 10, width: 300, height: 32 } },
                    "Text Input to Anchor flyout to:",
                    ' '),
                React.createElement(react_native_1.TextInput, { style: { height: 32, width: 250 }, ref: this._setRef })),
            this.state.isFlyoutVisible && (React.createElement(Flyout_1.Flyout, { isOpen: this.state.isFlyoutVisible, isLightDismissEnabled: this.state.isLightDismissEnabled, isOverlayEnabled: this.state.isOverlayEnabled, onDismiss: this._onFlyoutDismissed, target: this._anchor, placement: this.state.placementOptions },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightgray', width: 300, height: 400 } },
                    React.createElement(react_native_1.Text, { style: {
                            justifyContent: 'center',
                            paddingTop: 10,
                            paddingBottom: 30,
                        } }, "This is a flyout"),
                    React.createElement(react_native_1.Button, { onPress: this._onFlyoutButtonPressed, title: "Close" }),
                    React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                        React.createElement(react_native_1.Text, { style: { padding: 10 } }, "isLightDismissEnabled: "),
                        React.createElement(react_native_1.Switch, { style: { justifyContent: 'center', padding: 20 }, value: this.state.popupSwitchState, onValueChange: function (value) {
                                return _this.setState({
                                    popupSwitchState: value,
                                    isLightDismissEnabled: value,
                                });
                            } })),
                    React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                        React.createElement(react_native_1.Text, { style: { padding: 10 } }, "isOverlayEnabled: "),
                        React.createElement(react_native_1.Switch, { style: { justifyContent: 'center', padding: 20 }, value: this.state.isOverlayEnabled, onValueChange: function (value) {
                                return _this.setState({
                                    isOverlayEnabled: value,
                                });
                            } })),
                    React.createElement(react_native_1.TextInput, { style: { height: 32, width: 100 } }),
                    React.createElement(react_native_1.Text, { style: {
                            justifyContent: 'center',
                            paddingTop: 10,
                            marginLeft: 50,
                        } }, "Placement Options"),
                    React.createElement(PickerWindows_1.Picker, { style: {
                            borderBottomWidth: 1,
                            borderTopWidth: 1,
                            borderLeftWidth: 1,
                            borderRightWidth: 1,
                            width: 200,
                            marginLeft: 50,
                        }, selectedValue: this.state.placementOptions, onValueChange: function (value) {
                            return _this.setState({ placementOptions: value });
                        } }, placementValues.map(function (item) { return (React.createElement(PickerWindows_1.Picker.Item, { key: item, label: item, value: item })); })),
                    React.createElement(react_native_1.View, { style: {
                            width: 150,
                            marginLeft: 75,
                            marginTop: 10,
                        } },
                        React.createElement(react_native_1.Button, { onPress: this._onPressTwo, title: 'Open Another Flyout', ref: this._setRefTwo })),
                    React.createElement(react_native_1.View, { style: {
                            width: 150,
                            marginLeft: 75,
                            marginTop: 10,
                        } },
                        React.createElement(react_native_1.Button, { onPress: function () {
                                _this.setState({ isPopupVisible: true });
                            }, title: 'Open A Popup' }))))),
            this.state.isFlyoutTwoVisible && (React.createElement(Flyout_1.Flyout, { isOpen: this.state.isFlyoutTwoVisible, isLightDismissEnabled: true, onDismiss: this._onFlyoutTwoDismissed, target: this._anchorTwo, placement: this.state.placementOptions },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightblue', width: 200, height: 300 } },
                    React.createElement(react_native_1.Text, null, lorumIpsum)))),
            this.state.isPopupVisible && (React.createElement(Popup_1.Popup, { isOpen: this.state.isPopupVisible, isLightDismissEnabled: this.state.isLightDismissEnabled, target: this._anchorTwo, onDismiss: function () {
                    _this.setState({ isPopupVisible: false });
                } },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightblue', width: 200, height: 300 } },
                    React.createElement(react_native_1.Text, null, lorumIpsum),
                    React.createElement(react_native_1.Button, { onPress: function () {
                            _this.setState({ isPopupVisible: false });
                        }, title: "Close" }))))));
    };
    return FlyoutExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = '<Flyout>';
exports.description = 'Displays content on top of existing content, within the bounds of the application window.';
exports.examples = [
    {
        title: 'Flyout Anchor to text input',
        render: function () {
            return React.createElement(FlyoutExample, null);
        },
    },
];
//# sourceMappingURL=FlyoutExample.windows.js.map