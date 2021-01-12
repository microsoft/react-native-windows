"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.AccessibilityTestPage = void 0;
const react_native_1 = require("react-native");
const React = require("react");
const react_1 = require("react");
// MyView is a workaround. Currently in typescript, accessibilityAction doesn't allow name to be any string.
const MyView = (props) => React.createElement(react_native_1.View, Object.assign({}, props));
function AccessibilityTestPage() {
    const [pressedCountNested, setPressedCountNested] = react_1.useState(0);
    const [pressedCount, setPressedCount] = react_1.useState(0);
    const [accessibilityAction, setAccessibilityAction] = react_1.useState('');
    return (React.createElement(react_native_1.View, null,
        React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { accessible: true, accessibilityLiveRegion: "polite", style: { fontWeight: 'bold' } },
                "I'm bold",
                React.createElement(react_native_1.Text, { style: { color: 'red' }, onPress: () => setPressedCountNested(pressedCountNested + 1), accessible: true, accessibilityLiveRegion: "polite" },
                    "Pressed ",
                    pressedCountNested,
                    " times")),
            React.createElement(react_native_1.Text, { style: { color: 'green' }, onPress: () => setPressedCount(pressedCount + 1), accessible: true, accessibilityLiveRegion: "polite" },
                "Pressed ",
                pressedCount,
                " times")),
        React.createElement(react_native_1.View, null,
            React.createElement(MyView, { accessible: true, accessibilityLabel: "AccessibilityAction", accessibilityRole: "CheckBox", accessibilityState: { checked: true, expanded: true }, accessibilityActions: [
                    { name: 'toggle', label: 'toggle' },
                    { name: 'invoke', label: 'invoke' },
                    { name: 'expand', label: 'expand' },
                    { name: 'collapse', label: 'collapseIt' },
                ], onAccessibilityAction: (event) => {
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
exports.displayName = (_undefined) => { };
exports.title = 'LegacyAccessibilityTest';
exports.description = 'Legacy e2e test for Accessibility';
exports.examples = [
    {
        render: function () {
            return React.createElement(AccessibilityTestPage, null);
        },
    },
];
//# sourceMappingURL=AccessibilityTestPage.js.map