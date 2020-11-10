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
exports.examples = exports.description = exports.title = exports.displayName = exports.styles = exports.TextExample = exports.AttributeToggler = exports.Entity = void 0;
// This is a port of TextExample.android.js
// Image inline in Text removed
var React = require("react");
var react_native_1 = require("react-native");
var RNTesterBlock = require('../../components/RNTesterBlock');
var RNTesterPage = require('../../components/RNTesterPage');
var Entity = /** @class */ (function (_super) {
    __extends(Entity, _super);
    function Entity() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    Entity.prototype.render = function () {
        return (React.createElement(react_native_1.Text, { style: { fontWeight: 'bold', color: '#527fe4' } }, this.props.children));
    };
    return Entity;
}(React.Component));
exports.Entity = Entity;
var AttributeToggler = /** @class */ (function (_super) {
    __extends(AttributeToggler, _super);
    function AttributeToggler() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = { fontWeight: 'bold', fontSize: 15 };
        _this.toggleWeight = function () {
            _this.setState({
                fontWeight: _this.state.fontWeight === 'bold' ? 'normal' : 'bold',
            });
        };
        _this.increaseSize = function () {
            _this.setState({
                fontSize: _this.state.fontSize + 1,
            });
        };
        return _this;
    }
    AttributeToggler.prototype.render = function () {
        var curStyle = {
            fontWeight: this.state.fontWeight,
            fontSize: this.state.fontSize,
        };
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { style: curStyle }, "Tap the controls below to change attributes."),
            React.createElement(react_native_1.Text, null,
                React.createElement(react_native_1.Text, null,
                    "See how it will even work on",
                    ' ',
                    React.createElement(react_native_1.Text, { style: curStyle }, "this nested text"))),
            React.createElement(react_native_1.Text, null,
                React.createElement(react_native_1.Text, { onPress: this.toggleWeight }, "Toggle Weight"),
                ' (with highlight onPress)'),
            React.createElement(react_native_1.Text, { onPress: this.increaseSize, suppressHighlighting: true }, "Increase Size (suppressHighlighting true)")));
    };
    return AttributeToggler;
}(React.Component));
exports.AttributeToggler = AttributeToggler;
var TextExample = /** @class */ (function (_super) {
    __extends(TextExample, _super);
    function TextExample() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    TextExample.prototype.render = function () {
        var lorumIpsum = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed dapibus felis eget augue condimentum suscipit. Suspendisse hendrerit, libero aliquet malesuada tempor, urna nibh consectetur tellus, vitae efficitur quam erat non mi. Maecenas vitae eros sit amet quam vestibulum porta sed sit amet tellus. Fusce quis lectus congue, fringilla arcu id, luctus urna. Cras sagittis ornare mauris sit amet dictum. Vestibulum feugiat laoreet fringilla. Vivamus ac diam vehicula felis venenatis sagittis vitae ultrices elit. Curabitur libero augue, laoreet quis orci vitae, congue euismod massa. Aenean nec odio sed urna vehicula fermentum non a magna. Quisque ut commodo neque, eget eleifend odio. Sed sit amet lacinia sem. Suspendisse in metus in purus scelerisque vestibulum. Nam metus dui, efficitur nec metus non, tincidunt pharetra sapien. Praesent id convallis metus, ut malesuada arcu. Quisque quam libero, pharetra eu tellus ac, aliquam fringilla erat. Quisque tempus in lorem ac suscipit.';
        return (React.createElement(RNTesterPage, null,
            React.createElement(RNTesterBlock, { title: "textTransform" },
                React.createElement(react_native_1.View, null,
                    React.createElement(react_native_1.Text, { style: { textTransform: 'uppercase' } }, "This text should be uppercased."),
                    React.createElement(react_native_1.Text, { style: { textTransform: 'lowercase' } }, "This TEXT SHOULD be lowercased."),
                    React.createElement(react_native_1.Text, { style: { textTransform: 'capitalize' } }, "This text should be CAPITALIZED."),
                    React.createElement(react_native_1.Text, { style: { textTransform: 'capitalize' } },
                        "Mixed:",
                        ' ',
                        React.createElement(react_native_1.Text, { style: { textTransform: 'uppercase' } }, "uppercase "),
                        React.createElement(react_native_1.Text, { style: { textTransform: 'lowercase' } }, "LoWeRcAsE "),
                        React.createElement(react_native_1.Text, { style: { textTransform: 'capitalize' } }, "capitalize each word")),
                    React.createElement(react_native_1.Text, null,
                        "Should be \"ABC\":",
                        React.createElement(react_native_1.Text, { style: { textTransform: 'uppercase' } },
                            "a",
                            React.createElement(react_native_1.Text, null, "b"),
                            "c")),
                    React.createElement(react_native_1.Text, null,
                        "Should be \"XyZ\":",
                        React.createElement(react_native_1.Text, { style: { textTransform: 'uppercase' } },
                            "x",
                            React.createElement(react_native_1.Text, { style: { textTransform: 'none' } }, "y"),
                            "z")))),
            React.createElement(RNTesterBlock, { title: "Wrap" },
                React.createElement(react_native_1.Text, null,
                    "The text should wrap if it goes on multiple lines. See, this is going to the next line. ",
                    lorumIpsum)),
            React.createElement(RNTesterBlock, { title: "Padding" },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "This text is indented by 10px padding on all sides.")),
            React.createElement(RNTesterBlock, { title: "Font Family" },
                React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif' } }, "Sans-Serif"),
                React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif', fontWeight: 'bold' } }, "Sans-Serif Bold"),
                React.createElement(react_native_1.Text, { style: { fontFamily: 'serif' } }, "Serif"),
                React.createElement(react_native_1.Text, { style: { fontFamily: 'serif', fontWeight: 'bold' } }, "Serif Bold"),
                React.createElement(react_native_1.Text, { style: { fontFamily: 'monospace' } }, "Monospace"),
                React.createElement(react_native_1.Text, { style: { fontFamily: 'monospace', fontWeight: 'bold' } }, "Monospace Bold (After 5.0)")),
            React.createElement(RNTesterBlock, { title: "Android Material Design fonts" },
                React.createElement(react_native_1.View, { style: { flexDirection: 'row', alignItems: 'flex-start' } },
                    React.createElement(react_native_1.View, { style: { flex: 1 } },
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif' } }, "Roboto Regular"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif', fontStyle: 'italic' } }, "Roboto Italic"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif', fontWeight: 'bold' } }, "Roboto Bold"),
                        React.createElement(react_native_1.Text, { style: {
                                fontFamily: 'sans-serif',
                                fontStyle: 'italic',
                                fontWeight: 'bold',
                            } }, "Roboto Bold Italic"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif-light' } }, "Roboto Light"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif-light', fontStyle: 'italic' } }, "Roboto Light Italic"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif-thin' } }, "Roboto Thin (After 4.2)"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif-thin', fontStyle: 'italic' } }, "Roboto Thin Italic (After 4.2)"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif-condensed' } }, "Roboto Condensed"),
                        React.createElement(react_native_1.Text, { style: {
                                fontFamily: 'sans-serif-condensed',
                                fontStyle: 'italic',
                            } }, "Roboto Condensed Italic"),
                        React.createElement(react_native_1.Text, { style: {
                                fontFamily: 'sans-serif-condensed',
                                fontWeight: 'bold',
                            } }, "Roboto Condensed Bold"),
                        React.createElement(react_native_1.Text, { style: {
                                fontFamily: 'sans-serif-condensed',
                                fontStyle: 'italic',
                                fontWeight: 'bold',
                            } }, "Roboto Condensed Bold Italic"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif-medium' } }, "Roboto Medium (After 5.0)"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif-medium', fontStyle: 'italic' } }, "Roboto Medium Italic (After 5.0)")))),
            React.createElement(RNTesterBlock, { title: "Custom Fonts" },
                React.createElement(react_native_1.View, { style: { flexDirection: 'row', alignItems: 'flex-start' } },
                    React.createElement(react_native_1.View, { style: { flex: 1 } },
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'notoserif' } }, "NotoSerif Regular"),
                        React.createElement(react_native_1.Text, { style: {
                                fontFamily: 'notoserif',
                                fontStyle: 'italic',
                                fontWeight: 'bold',
                            } }, "NotoSerif Bold Italic"),
                        React.createElement(react_native_1.Text, { style: { fontFamily: 'notoserif', fontStyle: 'italic' } }, "NotoSerif Italic (Missing Font file)")))),
            React.createElement(RNTesterBlock, { title: "Font Size" },
                React.createElement(react_native_1.Text, { style: { fontSize: 23 } }, "Size 23"),
                React.createElement(react_native_1.Text, { style: { fontSize: 8 } }, "Size 8")),
            React.createElement(RNTesterBlock, { title: "Color" },
                React.createElement(react_native_1.Text, { style: { color: 'red' } }, "Red color"),
                React.createElement(react_native_1.Text, { style: { color: 'blue' } }, "Blue color")),
            React.createElement(RNTesterBlock, { title: "Font Weight" },
                React.createElement(react_native_1.Text, { style: { fontWeight: 'bold' } }, "Move fast and be bold"),
                React.createElement(react_native_1.Text, { style: { fontWeight: 'normal' } }, "Move fast and be bold")),
            React.createElement(RNTesterBlock, { title: "Font Style" },
                React.createElement(react_native_1.Text, { style: { fontStyle: 'italic' } }, "Move fast and be bold"),
                React.createElement(react_native_1.Text, { style: { fontStyle: 'normal' } }, "Move fast and be bold")),
            React.createElement(RNTesterBlock, { title: "Font Style and Weight" },
                React.createElement(react_native_1.Text, { style: { fontStyle: 'italic', fontWeight: 'bold' } }, "Move fast and be bold")),
            React.createElement(RNTesterBlock, { title: "Text Decoration" },
                React.createElement(react_native_1.Text, { style: { textDecorationLine: 'underline' } }, "Solid underline"),
                React.createElement(react_native_1.Text, { style: { textDecorationLine: 'none' } }, "None textDecoration"),
                React.createElement(react_native_1.Text, { style: {
                        textDecorationLine: 'line-through',
                        textDecorationStyle: 'solid',
                    } }, "Solid line-through"),
                React.createElement(react_native_1.Text, { style: { textDecorationLine: 'underline line-through' } }, "Both underline and line-through"),
                React.createElement(react_native_1.Text, null,
                    "Mixed text with",
                    ' ',
                    React.createElement(react_native_1.Text, { style: { textDecorationLine: 'underline' } }, "underline"),
                    " and",
                    ' ',
                    React.createElement(react_native_1.Text, { style: { textDecorationLine: 'line-through' } }, "line-through"),
                    ' ',
                    "text nodes")),
            React.createElement(RNTesterBlock, { title: "Nested" },
                React.createElement(react_native_1.Text, { onPress: function () { return console.log('1st'); } },
                    "(Normal text,",
                    React.createElement(react_native_1.Text, { style: { color: 'red', fontWeight: 'bold' } },
                        "(R)red",
                        React.createElement(react_native_1.Text, { style: { color: 'green', fontWeight: 'normal' } },
                            "(G)green",
                            React.createElement(react_native_1.Text, { style: { color: 'blue', fontWeight: 'bold' } },
                                "(B)blue",
                                React.createElement(react_native_1.Text, { style: { color: 'cyan', fontWeight: 'normal' } },
                                    "(C)cyan",
                                    React.createElement(react_native_1.Text, { style: { color: 'magenta', fontWeight: 'bold' } },
                                        "(M)magenta",
                                        React.createElement(react_native_1.Text, { style: { color: 'yellow', fontWeight: 'normal' } },
                                            "(Y)yellow",
                                            React.createElement(react_native_1.Text, { style: { color: 'black', fontWeight: 'bold' } }, "(K)black"))))))),
                    React.createElement(react_native_1.Text, { style: { fontWeight: 'bold' }, onPress: function () { return console.log('2nd'); } },
                        "(and bold",
                        React.createElement(react_native_1.Text, { style: { fontStyle: 'italic', fontSize: 11, color: '#527fe4' }, onPress: function () { return console.log('3rd'); } },
                            "(and tiny bold italic blue",
                            React.createElement(react_native_1.Text, { style: { fontWeight: 'normal', fontStyle: 'normal' }, onPress: function () { return console.log('4th'); } }, "(and tiny normal blue)"),
                            ")"),
                        ")"),
                    ")"),
                React.createElement(react_native_1.Text, { style: { fontFamily: 'serif' }, onPress: function () { return console.log('1st'); } },
                    "(Serif",
                    React.createElement(react_native_1.Text, { style: { fontStyle: 'italic', fontWeight: 'bold' }, onPress: function () { return console.log('2nd'); } },
                        "(Serif Bold Italic",
                        React.createElement(react_native_1.Text, { style: {
                                fontFamily: 'monospace',
                                fontStyle: 'normal',
                                fontWeight: 'normal',
                            }, onPress: function () { return console.log('3rd'); } },
                            "(Monospace Normal",
                            React.createElement(react_native_1.Text, { style: { fontFamily: 'sans-serif', fontWeight: 'bold' }, onPress: function () { return console.log('4th'); } },
                                "(Sans-Serif Bold",
                                React.createElement(react_native_1.Text, { style: { fontWeight: 'normal' }, onPress: function () { return console.log('5th'); } }, "(and Sans-Serif Normal)"),
                                ")"),
                            ")"),
                        ")"),
                    ")"),
                React.createElement(react_native_1.Text, { style: { fontSize: 12 } },
                    React.createElement(Entity, null, "Entity Name"))),
            React.createElement(RNTesterBlock, { title: "Text Align" },
                React.createElement(react_native_1.Text, null, "auto (default) - english LTR"),
                React.createElement(react_native_1.Text, null, "\u0623\u062D\u0628 \u0627\u0644\u0644\u063A\u0629 \u0627\u0644\u0639\u0631\u0628\u064A\u0629 auto (default) - arabic RTL"),
                React.createElement(react_native_1.Text, { style: { textAlign: 'left' } }, "left left left left left left left left left left left left left left left"),
                React.createElement(react_native_1.Text, { style: { textAlign: 'center' } }, "center center center center center center center center center center center"),
                React.createElement(react_native_1.Text, { style: { textAlign: 'right' } }, "right right right right right right right right right right right right right")),
            React.createElement(RNTesterBlock, { title: "Unicode" },
                React.createElement(react_native_1.View, null,
                    React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                        React.createElement(react_native_1.Text, { style: { backgroundColor: 'red' } }, "\u661F\u9645\u4E89\u9738\u662F\u4E16\u754C\u4E0A\u6700\u597D\u7684\u6E38\u620F\u3002")),
                    React.createElement(react_native_1.View, null,
                        React.createElement(react_native_1.Text, { style: { backgroundColor: 'red' } }, "\u661F\u9645\u4E89\u9738\u662F\u4E16\u754C\u4E0A\u6700\u597D\u7684\u6E38\u620F\u3002")),
                    React.createElement(react_native_1.View, { style: { alignItems: 'center' } },
                        React.createElement(react_native_1.Text, { style: { backgroundColor: 'red' } }, "\u661F\u9645\u4E89\u9738\u662F\u4E16\u754C\u4E0A\u6700\u597D\u7684\u6E38\u620F\u3002")),
                    React.createElement(react_native_1.View, null,
                        React.createElement(react_native_1.Text, { style: { backgroundColor: 'red' } }, "\u661F\u9645\u4E89\u9738\u662F\u4E16\u754C\u4E0A\u6700\u597D\u7684\u6E38\u620F\u3002\u661F\u9645\u4E89\u9738\u662F\u4E16\u754C\u4E0A\u6700\u597D\u7684\u6E38\u620F\u3002\u661F\u9645\u4E89\u9738\u662F\u4E16\u754C\u4E0A\u6700\u597D\u7684\u6E38\u620F\u3002\u661F\u9645\u4E89\u9738\u662F\u4E16\u754C\u4E0A\u6700\u597D\u7684\u6E38\u620F\u3002")))),
            React.createElement(RNTesterBlock, { title: "Spaces" },
                React.createElement(react_native_1.Text, null,
                    "A ",
                    'generated',
                    " ",
                    'string',
                    " and some \u00A0\u00A0\u00A0 spaces")),
            React.createElement(RNTesterBlock, { title: "Line Height" },
                React.createElement(react_native_1.Text, { style: { lineHeight: 35 } },
                    "Holisticly formulate inexpensive ideas before best-of-breed benefits. ",
                    React.createElement(react_native_1.Text, { style: { fontSize: 20 } }, "Continually"),
                    " expedite magnetic potentialities rather than client-focused interfaces.")),
            React.createElement(RNTesterBlock, { title: "Letter Spacing" },
                React.createElement(react_native_1.View, null,
                    React.createElement(react_native_1.Text, { style: { letterSpacing: 0 } }, "letterSpacing = 0"),
                    React.createElement(react_native_1.Text, { style: { letterSpacing: 2, marginTop: 5 } }, "letterSpacing = 2"),
                    React.createElement(react_native_1.Text, { style: { letterSpacing: 9, marginTop: 5 } }, "letterSpacing = 9"),
                    React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                        React.createElement(react_native_1.Text, { style: {
                                fontSize: 12,
                                letterSpacing: 2,
                                backgroundColor: 'fuchsia',
                                marginTop: 5,
                            } }, "With size and background color")),
                    React.createElement(react_native_1.Text, { style: { letterSpacing: -1, marginTop: 5 } }, "letterSpacing = -1"),
                    React.createElement(react_native_1.Text, { style: {
                            letterSpacing: 3,
                            backgroundColor: '#dddddd',
                            marginTop: 5,
                        } },
                        "[letterSpacing = 3]",
                        React.createElement(react_native_1.Text, { style: { letterSpacing: 0, backgroundColor: '#bbbbbb' } }, "[Nested letterSpacing = 0]"),
                        React.createElement(react_native_1.Text, { style: { letterSpacing: 6, backgroundColor: '#eeeeee' } }, "[Nested letterSpacing = 6]")))),
            React.createElement(RNTesterBlock, { title: "Empty Text" },
                React.createElement(react_native_1.Text, null)),
            React.createElement(RNTesterBlock, { title: "Toggling Attributes" },
                React.createElement(AttributeToggler, null)),
            React.createElement(RNTesterBlock, { title: "backgroundColor attribute" },
                React.createElement(react_native_1.Text, { style: { backgroundColor: '#ffaaaa' } },
                    "Red background,",
                    React.createElement(react_native_1.Text, { style: { backgroundColor: '#aaaaff' } },
                        ' ',
                        "blue background,",
                        React.createElement(react_native_1.Text, null,
                            ' ',
                            "inherited blue background,",
                            React.createElement(react_native_1.Text, { style: { backgroundColor: '#aaffaa' } },
                                ' ',
                                "nested green background.")))),
                React.createElement(react_native_1.Text, { style: { backgroundColor: 'rgba(100, 100, 100, 0.3)' } },
                    "Same alpha as background,",
                    React.createElement(react_native_1.Text, null,
                        "Inherited alpha from background,",
                        React.createElement(react_native_1.Text, { style: { backgroundColor: 'rgba(100, 100, 100, 0.3)' } }, "Reapply alpha")))),
            React.createElement(RNTesterBlock, { title: "containerBackgroundColor attribute" },
                React.createElement(react_native_1.View, { style: { flexDirection: 'row', height: 85 } },
                    React.createElement(react_native_1.View, { style: { backgroundColor: '#ffaaaa', width: 150 } }),
                    React.createElement(react_native_1.View, { style: { backgroundColor: '#aaaaff', width: 150 } })),
                React.createElement(react_native_1.Text, { style: [exports.styles.backgroundColorText, { top: -80 }] }, "Default containerBackgroundColor (inherited) + backgroundColor wash"),
                React.createElement(react_native_1.Text, { style: [
                        exports.styles.backgroundColorText,
                        { top: -70, backgroundColor: 'transparent' },
                    ] }, "containerBackgroundColor: 'transparent' + backgroundColor wash")),
            React.createElement(RNTesterBlock, { title: "numberOfLines attribute" },
                React.createElement(react_native_1.Text, { style: { marginTop: 0, fontStyle: 'italic' } }, "1"),
                React.createElement(react_native_1.Text, { numberOfLines: 1 },
                    "Maximum of one line no matter now much I write here. If I keep writing it",
                    "'",
                    "ll just truncate after one line. ",
                    lorumIpsum),
                React.createElement(react_native_1.Text, { style: { marginTop: 20, fontStyle: 'italic' } }, "2"),
                React.createElement(react_native_1.Text, { numberOfLines: 2 },
                    "Maximum of two lines no matter now much I write here. If I keep writing it",
                    "'",
                    "ll just truncate after two lines. ",
                    lorumIpsum),
                React.createElement(react_native_1.Text, { style: { marginTop: 20, fontStyle: 'italic' } }, "(default) infinity"),
                React.createElement(react_native_1.Text, null,
                    "No maximum lines specified no matter now much I write here. If I keep writing it",
                    "'",
                    "ll just keep going and going. ",
                    lorumIpsum)),
            React.createElement(RNTesterBlock, { title: "selectable attribute" },
                React.createElement(react_native_1.Text, { selectable: true }, "This text is selectable if you click-and-hold, and will offer the native Android selection menus.")),
            React.createElement(RNTesterBlock, { title: "selectionColor attribute" },
                React.createElement(react_native_1.Text, { selectable: true, selectionColor: "orange" }, "This text will have a orange highlight on selection.")),
            React.createElement(RNTesterBlock, { title: "Inline images" },
                React.createElement(react_native_1.Text, null,
                    "This text contains an inline image",
                    ' ',
                    " Neat, huh?")),
            React.createElement(RNTesterBlock, { title: "Text shadow" },
                React.createElement(react_native_1.Text, { style: {
                        fontSize: 20,
                        textShadowOffset: { width: 2, height: 2 },
                        textShadowRadius: 1,
                        textShadowColor: '#00cccc',
                    } }, "Demo text shadow")),
            React.createElement(RNTesterBlock, { title: "Ellipsize mode" },
                React.createElement(react_native_1.Text, { style: { marginTop: 0, fontStyle: 'italic' } }, "(default) tail"),
                React.createElement(react_native_1.Text, { numberOfLines: 1 },
                    "This very long text should be truncated with dots in the end.",
                    ' ',
                    lorumIpsum),
                React.createElement(react_native_1.Text, { style: { marginTop: 20, fontStyle: 'italic' } }, "middle"),
                React.createElement(react_native_1.Text, { ellipsizeMode: "middle", numberOfLines: 1 },
                    "This very long text should be truncated with dots in the middle.",
                    ' ',
                    lorumIpsum),
                React.createElement(react_native_1.Text, { style: { marginTop: 20, fontStyle: 'italic' } }, "head"),
                React.createElement(react_native_1.Text, { ellipsizeMode: "head", numberOfLines: 1 },
                    "This very long text should be truncated with dots in the beginning.",
                    ' ',
                    lorumIpsum),
                React.createElement(react_native_1.Text, { style: { marginTop: 20, fontStyle: 'italic' } }, "clip"),
                React.createElement(react_native_1.Text, { ellipsizeMode: "clip", numberOfLines: 1 },
                    "This very long text should be clipped and this will not be visible.",
                    ' ',
                    lorumIpsum)),
            React.createElement(RNTesterBlock, { title: "Include Font Padding" },
                React.createElement(react_native_1.View, { style: {
                        flexDirection: 'row',
                        justifyContent: 'space-around',
                        marginBottom: 10,
                    } },
                    React.createElement(react_native_1.View, { style: { alignItems: 'center' } },
                        React.createElement(react_native_1.Text, { style: exports.styles.includeFontPaddingText }, "Ey"),
                        React.createElement(react_native_1.Text, null, "Default")),
                    React.createElement(react_native_1.View, { style: { alignItems: 'center' } },
                        React.createElement(react_native_1.Text, { style: [
                                exports.styles.includeFontPaddingText,
                                { includeFontPadding: false, marginLeft: 10 },
                            ] }, "Ey"),
                        React.createElement(react_native_1.Text, null, "includeFontPadding: false"))),
                React.createElement(react_native_1.Text, null, "By default Android will put extra space above text to allow for upper-case accents or other ascenders. With some fonts, this can make text look slightly misaligned when centered vertically.")),
            React.createElement(RNTesterBlock, { title: "Text With Border" },
                React.createElement(React.Fragment, null,
                    React.createElement(react_native_1.Text, { style: exports.styles.borderedTextSimple }, "Sample bordered text with default styling."),
                    React.createElement(react_native_1.Text, { style: exports.styles.borderedText },
                        "Some more bordered text + a tad of CSS.",
                        '\n',
                        React.createElement(react_native_1.Text, { style: { borderColor: 'red', borderWidth: 5 } },
                            "1st nested - border specifcied but ignored.",
                            '\n',
                            React.createElement(react_native_1.Text, { style: { borderColor: 'yellow', borderWidth: 4 } }, "2nd Inside text!"))),
                    React.createElement(react_native_1.Text, null,
                        "This text is",
                        ' ',
                        React.createElement(react_native_1.Text, { style: { color: 'red', borderWidth: 1, borderColor: 'black' } },
                            "outlined",
                            ' '),
                        "and laid out within the normal text run, so will wrap etc as normal text.")))));
    };
    return TextExample;
}(React.Component));
exports.TextExample = TextExample;
exports.styles = react_native_1.StyleSheet.create({
    backgroundColorText: {
        left: 5,
        backgroundColor: 'rgba(100, 100, 100, 0.3)',
    },
    includeFontPaddingText: {
        fontSize: 120,
        fontFamily: 'sans-serif',
        backgroundColor: '#EEEEEE',
        color: '#000000',
        textAlignVertical: 'center',
        alignSelf: 'center',
    },
    borderedText: {
        margin: 100,
        fontSize: 18,
        fontWeight: 'bold',
        textAlign: 'center',
        borderWidth: 3,
        borderColor: 'green',
        padding: 30,
    },
    borderedTextSimple: {
        fontSize: 18,
        borderWidth: 2,
        borderColor: 'black',
        width: 400,
    },
});
exports.displayName = function (_undefined) { };
exports.title = '<Text>';
exports.description = 'Base component for rendering styled text.';
exports.examples = [
    {
        render: function () {
            return React.createElement(TextExample, null);
        },
    },
];
//# sourceMappingURL=TextExample.windows.js.map