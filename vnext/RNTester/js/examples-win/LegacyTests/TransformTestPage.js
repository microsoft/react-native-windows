"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.TransformTestPage = void 0;
var React = require("react");
var react_1 = require("react");
var react_native_1 = require("react-native");
var Consts_1 = require("./Consts");
var styles = react_native_1.StyleSheet.create({
    text: {
        textAlign: 'center',
        fontWeight: '700',
        height: 30,
    },
    childView: {
        width: 60,
        height: 60,
        marginTop: 50,
        backgroundColor: 'lightgreen',
        marginBottom: 20,
    },
});
var rootViewRef = React.createRef();
var childViewRef = React.createRef();
function TransformTestPage() {
    var _a = react_1.useState(''), resultTextState = _a[0], setResultTextState = _a[1];
    var _b = react_1.useState(1), viewScale = _b[0], setViewScale = _b[1];
    var measureLayoutSucceeded = function (x, y, width, height) {
        setResultTextState("x=" + x + ";y=" + y + ";width=" + Math.trunc(width) + ";height=" + Math.trunc(height));
    };
    var measureLayoutFailed = function () {
        setResultTextState('MeasureLayout failed');
    };
    return (React.createElement(react_native_1.View, { ref: rootViewRef },
        React.createElement(react_native_1.Text, { testID: Consts_1.TRANSFORM_TEST_RESULT }, resultTextState),
        React.createElement(react_native_1.View, { style: [styles.childView, { transform: [{ scale: viewScale }] }], ref: childViewRef }),
        React.createElement(react_native_1.Button, { title: "Apply ScaleTransform", onPress: function () {
                setViewScale(viewScale === 1 ? 0.5 : 1);
            }, testID: Consts_1.APPLY_SCALE_TRANSFORM_BUTTON }),
        React.createElement(react_native_1.Button, { title: "MeasureLayout", onPress: function () {
                if (childViewRef.current) {
                    var rootViewHandle = react_native_1.findNodeHandle(rootViewRef.current);
                    if (rootViewHandle) {
                        childViewRef.current.measureLayout(rootViewHandle, measureLayoutSucceeded, measureLayoutFailed);
                    }
                }
            }, testID: Consts_1.MEASURE_LAYOUT_BUTTON })));
}
exports.TransformTestPage = TransformTestPage;
exports.displayName = function (_undefined) { };
exports.title = '<LegacyTransformTest>';
exports.description = 'Legacy e2e test for Transforms';
exports.examples = [
    {
        render: function () {
            return React.createElement(TransformTestPage, null);
        },
    },
];
//# sourceMappingURL=TransformTestPage.js.map