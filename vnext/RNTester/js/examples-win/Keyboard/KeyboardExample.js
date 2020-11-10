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
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = void 0;
var React = require("react");
var react_native_1 = require("react-native");
var PickerWindows_1 = require("../../../../Libraries/Components/Picker/PickerWindows");
var TabStopExample = /** @class */ (function (_super) {
    __extends(TabStopExample, _super);
    function TabStopExample() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    TabStopExample.prototype.render = function () {
        var itemStyle = {
            width: 120,
            height: 50,
            backgroundColor: 'aqua',
            justifyContent: 'center',
            alignItems: 'center',
        };
        var pickerStyle = { width: 120, height: 50 };
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
                React.createElement(react_native_1.TouchableHighlight, __assign({ style: itemStyle }, { tabIndex: 1 }),
                    React.createElement(react_native_1.Text, null, "tabIndex 1")),
                React.createElement(react_native_1.TouchableHighlight, __assign({ style: itemStyle }, { tabIndex: 3 }),
                    React.createElement(react_native_1.Text, null, "tabIndex 3")),
                React.createElement(react_native_1.TouchableHighlight, __assign({ style: itemStyle }, { tabIndex: 2 }),
                    React.createElement(react_native_1.Text, null, "tabIndex 2"))),
            React.createElement(react_native_1.Text, null, "Controls like Picker should also do the same tab in the order of first, last, middle:"),
            React.createElement(react_native_1.View, { style: {
                    flex: 1,
                    flexDirection: 'row',
                    justifyContent: 'space-between',
                } },
                React.createElement(PickerWindows_1.Picker, __assign({ style: pickerStyle }, { tabIndex: 11 }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "tabIndex 11" })),
                React.createElement(PickerWindows_1.Picker, __assign({ style: pickerStyle }, { tabIndex: 13 }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "tabIndex 13" })),
                React.createElement(PickerWindows_1.Picker, __assign({ style: pickerStyle }, { tabIndex: 12 }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "tabIndex 12" })))));
    };
    return TabStopExample;
}(React.Component));
exports.displayName = function (_undefined) { };
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