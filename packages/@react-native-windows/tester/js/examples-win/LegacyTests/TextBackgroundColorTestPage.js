"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.TextBackgroundColorTestPage = void 0;
const React = require("react");
const react_native_1 = require("react-native");
const Consts_1 = require("./Consts");
const TreeDumpControl_1 = require("./TreeDumpControl");
function TextBackgroundColorTestPage() {
    return (React.createElement(react_native_1.View, null,
        React.createElement(react_native_1.View, { testID: 'TextColorBackgroundView' },
            React.createElement(react_native_1.Text, null,
                "Outer no_color",
                ' ',
                React.createElement(react_native_1.Text, { style: { backgroundColor: 'green' } },
                    "START_NESTED green",
                    ' ',
                    React.createElement(react_native_1.Text, { style: { backgroundColor: 'blue' } }, "DEEPER_NESTED blue"),
                    ' ',
                    "END_NESTED"),
                ' ',
                "attributes."),
            React.createElement(react_native_1.Text, null,
                "Outer no_color",
                ' ',
                React.createElement(react_native_1.Text, null,
                    "START_NESTED no_color",
                    ' ',
                    React.createElement(react_native_1.Text, { style: { backgroundColor: 'blue' } }, "DEEPER_NESTED blue"),
                    ' ',
                    "END_NESTED"),
                ' ',
                "attributes."),
            React.createElement(react_native_1.Text, null,
                "Outer no_color",
                ' ',
                React.createElement(react_native_1.Text, { style: { backgroundColor: 'green' } },
                    "START_NESTED green ",
                    React.createElement(react_native_1.Text, null, "DEEPER_NESTED inherit green"),
                    ' ',
                    "END_NESTED"),
                ' ',
                "attributes."),
            React.createElement(react_native_1.Text, { style: { backgroundColor: 'red' } },
                "Outer red",
                ' ',
                React.createElement(react_native_1.Text, null,
                    "START_NESTED inherit red ",
                    React.createElement(react_native_1.Text, null, "DEEPER_NESTED inherit red"),
                    ' ',
                    "END_NESTED"),
                ' ',
                "attributes."),
            React.createElement(react_native_1.Text, { style: { backgroundColor: 'red' } },
                "Outer red",
                ' ',
                React.createElement(react_native_1.Text, { style: { backgroundColor: 'green' } },
                    "START_NESTED green",
                    ' ',
                    React.createElement(react_native_1.Text, { style: { backgroundColor: 'blue' } }, "DEEPER_NESTED blue"),
                    ' ',
                    "END_NESTED"),
                ' ',
                "attributes.")),
        React.createElement(TreeDumpControl_1.TreeDumpControl, { style: {
                height: 150,
                width: 500,
                margin: 10,
            }, dumpID: 'TextColorBackground', uiaID: 'TextColorBackgroundView', testID: Consts_1.TREE_DUMP_RESULT, additionalProperties: ['TextHighlighters'] })));
}
exports.TextBackgroundColorTestPage = TextBackgroundColorTestPage;
exports.displayName = (_undefined) => { };
exports.title = '<LegacyTextBackgroundColorTest>';
exports.description = 'Legacy e2e test for Text with backgroundColor';
exports.examples = [
    {
        render: function () {
            return React.createElement(TextBackgroundColorTestPage, null);
        },
    },
];
//# sourceMappingURL=TextBackgroundColorTestPage.js.map