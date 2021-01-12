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
class AnchoredPopupExample extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            showPopup: false,
            buttonTitle: 'Open Popup',
        };
        this._togglePopup = () => {
            this.setState(state => ({
                buttonTitle: state.showPopup ? 'Open Popup' : 'Close Popup',
                showPopup: !state.showPopup,
            }));
        };
        this._onPopupDismissed = () => {
            this.setState({ buttonTitle: 'Open Popup', showPopup: false });
        };
        this._textInput = React.createRef();
    }
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { style: { width: 250 } }, "The following tests popup Anchor"),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10, width: 300, height: 32 } },
                    "Text Input to Anchor popup to:",
                    ' '),
                React.createElement(react_native_1.TextInput, { style: { height: 32, width: 300 }, ref: this._textInput })),
            React.createElement(react_native_1.View, { style: { justifyContent: 'center', padding: 50 } },
                React.createElement(react_native_1.Button, { onPress: this._togglePopup, title: this.state.buttonTitle })),
            this.state.showPopup && (React.createElement(react_native_windows_1.Popup, { isOpen: this.state.showPopup, onDismiss: this._onPopupDismissed, target: this._textInput.current, isLightDismissEnabled: true, horizontalOffset: 10, verticalOffset: 10 },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightgray', width: 200, height: 300 } },
                    React.createElement(react_native_1.Text, { style: {
                            justifyContent: 'center',
                            paddingTop: 10,
                            paddingBottom: 30,
                        } }, "This is a flyout"),
                    React.createElement(react_native_1.Button, { onPress: this._togglePopup, title: "Close" }))))));
    }
}
class PopupPlacementExample extends React.Component {
    constructor(props) {
        super(props);
        this._togglePopup = () => {
            this.setState(state => ({
                showPopup: !state.showPopup,
            }));
        };
        this._onPopupDismissed = () => {
            this.setState({ showPopup: false });
        };
        this._noop = () => {
            return;
        };
        this.state = {
            showPopup: false,
        };
    }
    render() {
        return (React.createElement(react_native_1.View, { style: { width: 500, height: 500 } },
            React.createElement(react_native_1.Button, { onPress: this._togglePopup, title: 'Toggle popup' }),
            this.state.showPopup && (React.createElement(react_native_windows_1.Popup, { style: { position: 'absolute', left: 0, right: 0, top: 0, bottom: 0 }, isOpen: this.state.showPopup, onDismiss: this._onPopupDismissed, isLightDismissEnabled: false },
                React.createElement(react_native_1.View, { style: { backgroundColor: 'lightgray', flex: 1 } },
                    React.createElement(react_native_1.Text, null, "This is a popup"),
                    React.createElement(react_native_1.Button, { onPress: this._togglePopup, title: "Toggle popup" }),
                    React.createElement(react_native_1.Button, { onPress: this._noop, title: "This is a button" }))))));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'Popup';
exports.category = 'UI';
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