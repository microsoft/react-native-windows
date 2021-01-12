"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.category = exports.title = exports.displayName = void 0;
const react_native_1 = require("react-native");
const React = require("react");
class WithoutCurlyBracketsExample extends React.Component {
    constructor(props) {
        super(props);
    }
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "Text without curly brackets")));
    }
}
class WithinCurlyBracketsExample extends React.Component {
    constructor(props) {
        super(props);
    }
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, 'text within curly brackets')));
    }
}
class InterpolationExample extends React.Component {
    constructor(props) {
        super(props);
    }
    render() {
        const someVariable = 'I am a variable';
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, 'I am a string / ' + someVariable)));
    }
}
class ChangingStateExample extends React.Component {
    constructor(props) {
        super(props);
        this.state = { text: 'initial state text' };
    }
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Button, { title: 'UPDATE STATE', onPress: () => this.setState({ text: 'updated state text' }) }),
            React.createElement(react_native_1.Text, null, 'non state text: ' + this.state.text)));
    }
}
class FastToSlowTextExample extends React.Component {
    constructor(props) {
        super(props);
        this.state = { text: '' };
    }
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Button, { title: 'UPDATE STATE', onPress: () => this.setState({ text: 'second string' }) }),
            React.createElement(react_native_1.Text, null,
                'first string',
                !!this.state.text && React.createElement(react_native_1.Text, null, this.state.text))));
    }
}
class SlowExamples extends React.Component {
    constructor(props) {
        super(props);
    }
    render() {
        const slowText = 'slowText';
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null,
                "I'm a ",
                slowText,
                " because my string interpolation needs to all be within a pair of curly brackets."),
            React.createElement(react_native_1.Text, null,
                "I'm",
                React.createElement(react_native_1.Text, null, "a slow text because I'm nested with other"),
                React.createElement(react_native_1.Text, null, 'texts'))));
    }
}
exports.displayName = undefined;
exports.title = 'Fast Path Texts';
exports.category = 'Basic';
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