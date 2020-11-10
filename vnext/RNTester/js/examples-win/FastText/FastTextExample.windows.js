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
var react_native_1 = require("react-native");
var React = require("react");
var WithoutCurlyBracketsExample = /** @class */ (function (_super) {
    __extends(WithoutCurlyBracketsExample, _super);
    function WithoutCurlyBracketsExample(props) {
        return _super.call(this, props) || this;
    }
    WithoutCurlyBracketsExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "Text without curly brackets")));
    };
    return WithoutCurlyBracketsExample;
}(React.Component));
var WithinCurlyBracketsExample = /** @class */ (function (_super) {
    __extends(WithinCurlyBracketsExample, _super);
    function WithinCurlyBracketsExample(props) {
        return _super.call(this, props) || this;
    }
    WithinCurlyBracketsExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, 'text within curly brackets')));
    };
    return WithinCurlyBracketsExample;
}(React.Component));
var InterpolationExample = /** @class */ (function (_super) {
    __extends(InterpolationExample, _super);
    function InterpolationExample(props) {
        return _super.call(this, props) || this;
    }
    InterpolationExample.prototype.render = function () {
        var someVariable = 'I am a variable';
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, 'I am a string / ' + someVariable)));
    };
    return InterpolationExample;
}(React.Component));
var ChangingStateExample = /** @class */ (function (_super) {
    __extends(ChangingStateExample, _super);
    function ChangingStateExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { text: 'initial state text' };
        return _this;
    }
    ChangingStateExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Button, { title: 'UPDATE STATE', onPress: function () { return _this.setState({ text: 'updated state text' }); } }),
            React.createElement(react_native_1.Text, null, 'non state text: ' + this.state.text)));
    };
    return ChangingStateExample;
}(React.Component));
var FastToSlowTextExample = /** @class */ (function (_super) {
    __extends(FastToSlowTextExample, _super);
    function FastToSlowTextExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { text: '' };
        return _this;
    }
    FastToSlowTextExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Button, { title: 'UPDATE STATE', onPress: function () { return _this.setState({ text: 'second string' }); } }),
            React.createElement(react_native_1.Text, null,
                'first string',
                !!this.state.text && React.createElement(react_native_1.Text, null, this.state.text))));
    };
    return FastToSlowTextExample;
}(React.Component));
var SlowExamples = /** @class */ (function (_super) {
    __extends(SlowExamples, _super);
    function SlowExamples(props) {
        return _super.call(this, props) || this;
    }
    SlowExamples.prototype.render = function () {
        var slowText = 'slowText';
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null,
                "I'm a ",
                slowText,
                " because my string interpolation needs to all be within a pair of curly brackets."),
            React.createElement(react_native_1.Text, null,
                "I'm",
                React.createElement(react_native_1.Text, null, "a slow text because I'm nested with other"),
                React.createElement(react_native_1.Text, null, 'texts'))));
    };
    return SlowExamples;
}(React.Component));
exports.displayName = undefined;
exports.title = 'Fast Path Texts';
exports.description = 'Examples of performant fast path texts, turn on IsTextPerformanceVisualizationEnabled to visualize examples';
exports.examples = [
    {
        title: 'Without curly brackets example',
        render: function () {
            return React.createElement(WithoutCurlyBracketsExample, null);
        },
    },
    {
        title: 'Within curly brackets example',
        render: function () {
            return React.createElement(WithinCurlyBracketsExample, null);
        },
    },
    {
        title: 'String interpolation example',
        render: function () {
            return React.createElement(InterpolationExample, null);
        },
    },
    {
        title: 'Changing states within text example',
        render: function () {
            return React.createElement(ChangingStateExample, null);
        },
    },
    {
        title: 'Fast to slow text example',
        render: function () {
            return React.createElement(FastToSlowTextExample, null);
        },
    },
    {
        title: 'Slow path text examples',
        render: function () {
            return React.createElement(SlowExamples, null);
        },
    },
];
//# sourceMappingURL=FastTextExample.windows.js.map