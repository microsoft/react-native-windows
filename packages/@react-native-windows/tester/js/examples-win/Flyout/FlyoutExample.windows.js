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
const placementValues = [
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
class FlyoutExample extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            isFlyoutVisible: false,
            isFlyoutTwoVisible: false,
            isPopupVisible: false,
            buttonTitle: 'Open Flyout',
            isLightDismissEnabled: true,
            isOverlayEnabled: false,
            popupSwitchState: true,
            placementOptions: 'top',
        };
        this._setRef = (textInput) => {
            this._anchor = textInput;
        };
        this._onPress = () => {
            this.setState({ buttonTitle: 'Close Flyout', isFlyoutVisible: true });
        };
        this._onFlyoutButtonPressed = () => {
            this.setState({ buttonTitle: 'Open Flyout', isFlyoutVisible: false });
        };
        this._onFlyoutDismissed = (_isOpen) => {
            this.setState({ isFlyoutVisible: false });
            this.setState({ isFlyoutTwoVisible: false });
            this.setState({ isPopupVisible: false });
            this.setState({ buttonTitle: 'Open Flyout' });
        };
        this._setRefTwo = (element) => {
            this._anchorTwo = element;
        };
        this._onPressTwo = () => {
            this.setState({ isFlyoutTwoVisible: true });
        };
        this._onFlyoutTwoDismissed = (_isOpen) => {
            this.setState({ isFlyoutTwoVisible: false });
        };
        this._anchor = React.createRef();
        this._anchorTwo = React.createRef();
    }
    render() {
        const lorumIpsum = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed dapibus felis eget augue condimentum suscipit. Suspendisse hendrerit, libero aliquet malesuada tempor, urna nibh consectetur tellus, vitae efficitur quam erat non mi. Maecenas vitae eros sit amet quam vestibulum porta sed sit amet tellus. Fusce quis lectus congue, fringilla arcu id, luctus urna. Cras sagittis ornare mauris sit amet dictum. Vestibulum feugiat laoreet fringilla. Vivamus ac diam vehicula felis venenatis sagittis vitae ultrices elit. Curabitur libero augue, laoreet quis orci vitae, congue euismod massa. Aenean nec odio sed urna vehicula fermentum non a magna. Quisque ut commodo neque, eget eleifend odio. Sed sit amet lacinia sem. Suspendisse in metus in purus scelerisque vestibulum. Nam metus dui, efficitur nec metus non, tincidunt pharetra sapien. Praesent id convallis metus, ut malesuada arcu. Quisque quam libero, pharetra eu tellus ac, aliquam fringilla erat. Quisque tempus in lorem ac suscipit.';
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.View, { style: { flexDirection: 'row', paddingTop: 20 } },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "Placement Options: "),
                React.createElement(react_native_windows_1.Picker, { style: { width: 200, height: 35 }, selectedValue: this.state.placementOptions, onValueChange: value => this.setState({ placementOptions: value }) }, placementValues.map(item => (React.createElement(react_native_windows_1.Picker.Item, { key: item, label: item, value: item }))))),
            React.createElement(react_native_1.View, { style: { justifyContent: 'center', padding: 20, width: 200 } },
                React.createElement(react_native_1.Button, { onPress: this._onPress, title: this.state.buttonTitle })),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row', paddingTop: 200 } },
                React.createElement(react_native_1.Text, { style: { padding: 10, width: 300, height: 32 } },
                    "Text Input to Anchor flyout to:",
                    ' '),
                React.createElement(react_native_1.TextInput, { style: { height: 32, width: 250 }, ref: this._setRef })),
            this.state.isFlyoutVisible && (React.createElement(react_native_windows_1.Flyout, { isOpen: this.state.isFlyoutVisible, isLightDismissEnabled: this.state.isLightDismissEnabled, isOverlayEnabled: this.state.isOverlayEnabled, onDismiss: this._onFlyoutDismissed, target: this._anchor, placement: this.state.placementOptions },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightgray', width: 300, height: 400 } },
                    React.createElement(react_native_1.Text, { style: {
                            justifyContent: 'center',
                            paddingTop: 10,
                            paddingBottom: 30,
                        } }, "This is a flyout"),
                    React.createElement(react_native_1.Button, { onPress: this._onFlyoutButtonPressed, title: "Close" }),
                    React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                        React.createElement(react_native_1.Text, { style: { padding: 10 } }, "isLightDismissEnabled: "),
                        React.createElement(react_native_1.Switch, { style: { justifyContent: 'center', padding: 20 }, value: this.state.popupSwitchState, onValueChange: value => this.setState({
                                popupSwitchState: value,
                                isLightDismissEnabled: value,
                            }) })),
                    React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                        React.createElement(react_native_1.Text, { style: { padding: 10 } }, "isOverlayEnabled: "),
                        React.createElement(react_native_1.Switch, { style: { justifyContent: 'center', padding: 20 }, value: this.state.isOverlayEnabled, onValueChange: value => this.setState({
                                isOverlayEnabled: value,
                            }) })),
                    React.createElement(react_native_1.TextInput, { style: { height: 32, width: 100 } }),
                    React.createElement(react_native_1.Text, { style: {
                            justifyContent: 'center',
                            paddingTop: 10,
                            marginLeft: 50,
                        } }, "Placement Options"),
                    React.createElement(react_native_windows_1.Picker, { style: {
                            borderBottomWidth: 1,
                            borderTopWidth: 1,
                            borderLeftWidth: 1,
                            borderRightWidth: 1,
                            width: 200,
                            marginLeft: 50,
                        }, selectedValue: this.state.placementOptions, onValueChange: value => this.setState({ placementOptions: value }) }, placementValues.map(item => (React.createElement(react_native_windows_1.Picker.Item, { key: item, label: item, value: item })))),
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
                        React.createElement(react_native_1.Button, { onPress: () => {
                                this.setState({ isPopupVisible: true });
                            }, title: 'Open A Popup' }))))),
            this.state.isFlyoutTwoVisible && (React.createElement(react_native_windows_1.Flyout, { isOpen: this.state.isFlyoutTwoVisible, isLightDismissEnabled: true, onDismiss: this._onFlyoutTwoDismissed, target: this._anchorTwo, placement: this.state.placementOptions },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightblue', width: 200, height: 300 } },
                    React.createElement(react_native_1.Text, null, lorumIpsum)))),
            this.state.isPopupVisible && (React.createElement(react_native_windows_1.Popup, { isOpen: this.state.isPopupVisible, isLightDismissEnabled: this.state.isLightDismissEnabled, target: this._anchorTwo, onDismiss: () => {
                    this.setState({ isPopupVisible: false });
                } },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightblue', width: 200, height: 300 } },
                    React.createElement(react_native_1.Text, null, lorumIpsum),
                    React.createElement(react_native_1.Button, { onPress: () => {
                            this.setState({ isPopupVisible: false });
                        }, title: "Close" }))))));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'Flyout';
exports.category = 'Basic';
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