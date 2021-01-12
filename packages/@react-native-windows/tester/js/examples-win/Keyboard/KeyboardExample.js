"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = void 0;
const React = require("react");
const react_native_1 = require("react-native");
const react_native_windows_1 = require("react-native-windows");
class TabStopExample extends React.Component {
    render() {
        const itemStyle = {
            width: 120,
            height: 50,
            backgroundColor: 'aqua',
            justifyContent: 'center',
            alignItems: 'center',
        };
        const pickerStyle = { width: 120, height: 50 };
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "No tab index, this item will be tabbed to last"),
            React.createElement(react_native_1.TouchableHighlight, { style: itemStyle },
                React.createElement(react_native_1.Text, null, "tabIndex default")),
            React.createElement(react_native_1.Text, null, "These 3 items should tab in the order of first, last, middle:"),
            React.createElement(react_native_1.View, { style: {
                    flex: 1,
                    flexDirection: 'row',
                    justifyContent: 'space-between',
                } },
                React.createElement(react_native_1.TouchableHighlight, Object.assign({ style: itemStyle }, { tabIndex: 1 }),
                    React.createElement(react_native_1.Text, null, "tabIndex 1")),
                React.createElement(react_native_1.TouchableHighlight, Object.assign({ style: itemStyle }, { tabIndex: 3 }),
                    React.createElement(react_native_1.Text, null, "tabIndex 3")),
                React.createElement(react_native_1.TouchableHighlight, Object.assign({ style: itemStyle }, { tabIndex: 2 }),
                    React.createElement(react_native_1.Text, null, "tabIndex 2"))),
            React.createElement(react_native_1.Text, null, "Controls like Picker should also do the same tab in the order of first, last, middle:"),
            React.createElement(react_native_1.View, { style: {
                    flex: 1,
                    flexDirection: 'row',
                    justifyContent: 'space-between',
                } },
                React.createElement(react_native_windows_1.Picker, Object.assign({ style: pickerStyle }, { tabIndex: 11 }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "tabIndex 11" })),
                React.createElement(react_native_windows_1.Picker, Object.assign({ style: pickerStyle }, { tabIndex: 13 }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "tabIndex 13" })),
                React.createElement(react_native_windows_1.Picker, Object.assign({ style: pickerStyle }, { tabIndex: 12 }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "tabIndex 12" })))));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'Keyboard';
exports.description = 'Usage of keyboard properties.';
exports.examples = [
    {
        title: 'Tabstops',
        render: function () {
            return React.createElement(TabStopExample, null);
        },
    },
];
//# sourceMappingURL=KeyboardExample.js.map