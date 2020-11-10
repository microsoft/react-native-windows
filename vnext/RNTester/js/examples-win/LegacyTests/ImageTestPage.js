"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.ImageTestPage = void 0;
var react_native_1 = require("react-native");
var React = require("react");
var react_1 = require("react");
var Consts_1 = require("./Consts");
var TreeDumpControl_1 = require("./TreeDumpControl");
var styles = react_native_1.StyleSheet.create({
    container: {
        height: 300,
        width: 500,
        backgroundColor: 'yellow',
        alignItems: 'center',
    },
    containerWithBorder: {
        height: 300,
        width: 500,
        borderRadius: 10.0,
        borderWidth: 10,
        borderColor: '#00ff0055',
        backgroundColor: 'yellow',
        alignItems: 'center',
    },
    imageWithBorder: {
        height: '100%',
        width: '100%',
        borderRadius: 10.0,
        borderWidth: 10,
        borderColor: '#0000ff55',
        backgroundColor: 'red',
    },
    image: {
        height: '100%',
        width: '100%',
        backgroundColor: 'red',
    },
    rtlImage: {
        height: '100%',
        width: '100%',
        backgroundColor: 'red',
        direction: 'rtl',
    },
    treeDumpControl: {
        height: 150,
        width: 500,
        margin: 10,
    },
});
function ImageTestPage() {
    var _a = react_1.useState(false), imageWithBorder = _a[0], setImageBorder = _a[1];
    var _b = react_1.useState(false), rltMode = _b[0], setRtlMode = _b[1];
    var _c = react_1.useState(0), clickCount = _c[0], setClickCount = _c[1];
    var onPressBorder = function () {
        var previousImageBorderState = imageWithBorder;
        setImageBorder(!previousImageBorderState);
        var previousClickCount = clickCount;
        setClickCount(previousClickCount + 1);
    };
    var imageStyle = function () {
        if (rltMode) {
            return styles.rtlImage;
        }
        if (imageWithBorder) {
            return styles.imageWithBorder;
        }
        return styles.image;
    };
    var dumpId = function () {
        if (rltMode) {
            return 'ImageRTL';
        }
        if (imageWithBorder) {
            return 'ImageWithBorder';
        }
        if (clickCount === 0) {
            return 'ImageWithoutBorder';
        }
        return 'ImageWithoutBorder-Subsequent';
    };
    return (React.createElement(react_native_1.View, null,
        React.createElement(react_native_1.View, { testID: Consts_1.IMAGE_CONTAINER, style: imageWithBorder ? styles.containerWithBorder : styles.container },
            React.createElement(react_native_1.Image, { style: imageStyle(), resizeMode: 'center', source: {
                    uri: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAAzCAYAAAA6oTAqAAAAEXRFWHRTb2Z0d2FyZQBwbmdjcnVzaEB1SfMAAABQSURBVGje7dSxCQBACARB+2/ab8BEeQNhFi6WSYzYLYudDQYGBgYGBgYGBgYGBgYGBgZmcvDqYGBgmhivGQYGBgYGBgYGBgYGBgYGBgbmQw+P/eMrC5UTVAAAAABJRU5ErkJggg==',
                } })),
        React.createElement(react_native_1.Button, { title: imageWithBorder ? 'Hide Border' : 'Show Border', onPress: onPressBorder, testID: Consts_1.SHOW_IMAGE_BORDER }),
        React.createElement(react_native_1.Button, { title: rltMode ? 'Set image to LTR' : 'Set image to RTL', onPress: function () { return setRtlMode(!rltMode); }, testID: Consts_1.SET_RTL_MODE }),
        React.createElement(TreeDumpControl_1.TreeDumpControl, { style: styles.treeDumpControl, dumpID: dumpId(), uiaID: Consts_1.IMAGE_CONTAINER, additionalProperties: rltMode ? ['FlowDirection'] : [], testID: Consts_1.TREE_DUMP_RESULT })));
}
exports.ImageTestPage = ImageTestPage;
exports.displayName = function (_undefined) { };
exports.title = '<LegacyImageTest>';
exports.description = 'Legacy e2e test for Image';
exports.examples = [
    {
        render: function () {
            return React.createElement(ImageTestPage, null);
        },
    },
];
//# sourceMappingURL=ImageTestPage.js.map