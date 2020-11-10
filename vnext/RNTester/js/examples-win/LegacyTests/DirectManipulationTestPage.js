"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.DirectManipulationTestPage = void 0;
var React = require("react");
var react_1 = require("react");
var react_native_1 = require("react-native");
var Consts_1 = require("./Consts");
var styles = react_native_1.StyleSheet.create({
    container: {
        padding: 20,
    },
    text: {
        textAlign: 'center',
        fontWeight: '700',
        height: 30,
    },
    childView: {
        width: 50,
        height: 50,
        backgroundColor: 'lightgreen',
        marginBottom: 20,
    },
});
var rootViewRef = React.createRef();
var childViewRef = React.createRef();
function DirectManipulationTestPage() {
    var _a = react_1.useState(''), resultTextState = _a[0], setResultTextState = _a[1];
    var measureLayoutSucceeded = function (x, y, width, height) {
        setResultTextState("x=" + x + ";y=" + y + ";width=" + Math.trunc(width) + ";height=" + Math.trunc(height));
    };
    var measureLayoutFailed = function () {
        setResultTextState('MeasureLayout failed');
    };
    return (React.createElement(react_native_1.View, { ref: rootViewRef, style: styles.container },
        React.createElement(react_native_1.Text, { style: styles.text }, "DirectManipulationResult:"),
        React.createElement(react_native_1.Text, { testID: Consts_1.DIRECT_MANIPULATION_RESULT }, resultTextState),
        React.createElement(react_native_1.View, { style: styles.childView, ref: childViewRef }),
        React.createElement(react_native_1.Button, { title: "Call MeasureInWindow", onPress: function () {
                rootViewRef.current &&
                    rootViewRef.current.measureInWindow(function (x, y, width, height) {
                        setResultTextState("x=" + x + ";y=" + y + ";width=" + width + ";height=" + height);
                    });
            }, testID: Consts_1.MEASURE_IN_WINDOW_BUTTON }),
        React.createElement(react_native_1.Button, { title: "Call MeasureLayout", onPress: function () {
                if (childViewRef.current) {
                    var rootViewHandle = react_native_1.findNodeHandle(rootViewRef.current);
                    if (rootViewHandle) {
                        childViewRef.current.measureLayout(rootViewHandle, measureLayoutSucceeded, measureLayoutFailed);
                    }
                }
            }, testID: Consts_1.MEASURE_LAYOUT_BUTTON })));
}
exports.DirectManipulationTestPage = DirectManipulationTestPage;
exports.displayName = function (_undefined) { };
exports.title = '<LegacyDirectManipulationTest>';
exports.description = 'Legacy e2e test for DirectManipulation';
exports.examples = [
    {
        render: function () {
            return React.createElement(DirectManipulationTestPage, null);
        },
    },
];
//# sourceMappingURL=DirectManipulationTestPage.js.map