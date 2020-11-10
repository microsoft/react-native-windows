"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
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
exports.examples = exports.description = exports.title = exports.displayName = exports.AccessibilityTestPage = void 0;
var react_native_1 = require("react-native");
var React = require("react");
var react_1 = require("react");
// MyView is a workaround. Currently in typescript, accessibilityAction doesn't allow name to be any string.
var MyView = function (props) { return React.createElement(react_native_1.View, __assign({}, props)); };
function AccessibilityTestPage() {
    var _a = react_1.useState(0), pressedCountNested = _a[0], setPressedCountNested = _a[1];
    var _b = react_1.useState(0), pressedCount = _b[0], setPressedCount = _b[1];
    var _c = react_1.useState(''), accessibilityAction = _c[0], setAccessibilityAction = _c[1];
    return (React.createElement(react_native_1.View, null,
        React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { accessible: true, accessibilityLiveRegion: "polite", style: { fontWeight: 'bold' } },
                "I'm bold",
                React.createElement(react_native_1.Text, { style: { color: 'red' }, onPress: function () { return setPressedCountNested(pressedCountNested + 1); }, accessible: true, accessibilityLiveRegion: "polite" },
                    "Pressed ",
                    pressedCountNested,
                    " times")),
            React.createElement(react_native_1.Text, { style: { color: 'green' }, onPress: function () { return setPressedCount(pressedCount + 1); }, accessible: true, accessibilityLiveRegion: "polite" },
                "Pressed ",
                pressedCount,
                " times")),
        React.createElement(react_native_1.View, null,
            React.createElement(MyView, { accessible: true, accessibilityLabel: "AccessibilityAction", accessibilityRole: "CheckBox", accessibilityState: { checked: true, expanded: true }, accessibilityActions: [
                    { name: 'toggle', label: 'toggle' },
                    { name: 'invoke', label: 'invoke' },
                    { name: 'expand', label: 'expand' },
                    { name: 'collapse', label: 'collapseIt' },
                ], onAccessibilityAction: function (event) {
                    switch (event.nativeEvent.actionName) {
                        case 'toggle':
                            setAccessibilityAction('toggle action success');
                            break;
                        case 'invoke':
                            setAccessibilityAction('invoke action success');
                            break;
                        case 'expand':
                            setAccessibilityAction('expand action success');
                            break;
                        case 'collapseIt':
                            setAccessibilityAction('collapseIt action success');
                            break;
                    }
                } },
                React.createElement(react_native_1.Text, null,
                    "accessibilityAction:",
                    accessibilityAction)))));
}
exports.AccessibilityTestPage = AccessibilityTestPage;
exports.displayName = function (_undefined) { };
exports.title = '<LegacyAccessibilityTest>';
exports.description = 'Legacy e2e test for Accessibility';
exports.examples = [
    {
        render: function () {
            return React.createElement(AccessibilityTestPage, null);
        },
    },
];
//# sourceMappingURL=AccessibilityTestPage.js.map