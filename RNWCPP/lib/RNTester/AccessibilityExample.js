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
/* tslint:disable */
var React = require("react");
var react_native_1 = require("react-native");
var AccessibilityBaseExample = /** @class */ (function (_super) {
    __extends(AccessibilityBaseExample, _super);
    function AccessibilityBaseExample() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    AccessibilityBaseExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "The following has accessibilityLabel and accessibilityHint:"),
            React.createElement(react_native_1.View, { style: { width: 50, height: 50, backgroundColor: 'blue' }, accessibilityLabel: "A blue box", accessibilityHint: "A hint for the blue box." }),
            React.createElement(react_native_1.Text, null, "The following has accessible and accessibilityLabel:"),
            React.createElement(react_native_1.View, { style: { width: 50, height: 50, backgroundColor: 'red' }, accessible: true, accessibilityLabel: "A hint for the red box." })));
    };
    return AccessibilityBaseExample;
}(React.Component));
var TouchableExamples = /** @class */ (function (_super) {
    __extends(TouchableExamples, _super);
    function TouchableExamples() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            pressedCount: 0,
        };
        _this.press = function () {
            _this.setState({ pressedCount: _this.state.pressedCount + 1 });
        };
        return _this;
    }
    TouchableExamples.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "The following TouchableHighlight has accessibilityLabel, accessibilityHint, accessibilityRole, toolip:"),
            React.createElement(react_native_1.TouchableHighlight, __assign({ style: { width: 50, height: 50, backgroundColor: 'blue' }, accessibilityLabel: "A blue box", accessibilityHint: "A hint for the blue box.", accessibilityRole: "button", onPress: this.press }, { tooltip: "a blue tooltip" }),
                React.createElement(react_native_1.Text, null, "Blue")),
            React.createElement(react_native_1.Text, null,
                "Pressed ",
                this.state.pressedCount,
                " times")));
    };
    return TouchableExamples;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = 'Accessibility';
exports.description = 'Usage of accessibility properties.';
exports.examples = [
    {
        title: 'Label, Hint',
        render: function () {
            return React.createElement(AccessibilityBaseExample, null);
        },
    },
    {
        title: 'Touchables',
        render: function () {
            return React.createElement(TouchableExamples, null);
        },
    }
];
//# sourceMappingURL=AccessibilityExample.js.map