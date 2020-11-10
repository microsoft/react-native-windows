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
var Glyph_1 = require("../../../../Libraries/Components/Glyph/Glyph");
var GlyphExamples = /** @class */ (function (_super) {
    __extends(GlyphExamples, _super);
    function GlyphExamples() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    GlyphExamples.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "Glyph: default Accent color"),
            React.createElement(Glyph_1.Glyph, { style: { width: 32, height: 32 }, emSize: 24, fontUri: "ms-appx:///Assets/seguiemj.ttf", glyph: "300" }),
            React.createElement(react_native_1.Text, null, "Glyph: red"),
            React.createElement(Glyph_1.Glyph, { style: { width: 32, height: 32, color: 'red' }, emSize: 24, fontUri: "ms-appx:///Assets/seguiemj.ttf", glyph: "300" }),
            React.createElement(react_native_1.Text, null, "Multiple glyphs: red"),
            React.createElement(Glyph_1.Glyph, { style: { width: 150, height: 32, color: 'red' }, emSize: 24, fontUri: "ms-appx:///Assets/seguiemj.ttf", glyph: "300;304;703;802;803" }),
            React.createElement(react_native_1.Text, null, "Multiple glyphs"),
            React.createElement(Glyph_1.Glyph, { style: { width: 150, height: 32, color: 'red' }, emSize: 24, colorEnabled: true, fontUri: "ms-appx:///Assets/seguiemj.ttf", glyph: "804;805;806;807;808;809" })));
    };
    return GlyphExamples;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = '<Glyph> UWP';
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