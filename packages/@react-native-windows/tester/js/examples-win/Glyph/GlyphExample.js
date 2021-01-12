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
class GlyphExamples extends React.Component {
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "Glyph: default Accent color"),
            React.createElement(react_native_windows_1.Glyph, { style: { width: 32, height: 32 }, emSize: 24, fontUri: "ms-appx:///Assets/seguiemj.ttf", glyph: "300" }),
            React.createElement(react_native_1.Text, null, "Glyph: red"),
            React.createElement(react_native_windows_1.Glyph, { style: { width: 32, height: 32, color: 'red' }, emSize: 24, fontUri: "ms-appx:///Assets/seguiemj.ttf", glyph: "300" }),
            React.createElement(react_native_1.Text, null, "Multiple glyphs: red"),
            React.createElement(react_native_windows_1.Glyph, { style: { width: 150, height: 32, color: 'red' }, emSize: 24, fontUri: "ms-appx:///Assets/seguiemj.ttf", glyph: "300;304;703;802;803" }),
            React.createElement(react_native_1.Text, null, "Multiple glyphs"),
            React.createElement(react_native_windows_1.Glyph, { style: { width: 150, height: 32, color: 'red' }, emSize: 24, colorEnabled: true, fontUri: "ms-appx:///Assets/seguiemj.ttf", glyph: "804;805;806;807;808;809" })));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'Glyph UWP';
exports.category = 'UI';
exports.description = 'Usage of Glyph control.';
exports.examples = [
    {
        title: 'Glyph examples',
        render: function () {
            return React.createElement(GlyphExamples, null);
        },
    },
];
//# sourceMappingURL=GlyphExample.js.map