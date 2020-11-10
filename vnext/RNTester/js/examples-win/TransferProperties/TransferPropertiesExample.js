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
var styles = react_native_1.StyleSheet.create({
    withBorder: {
        borderRadius: 5,
    },
    withoutBorder: {
        direction: 'rtl',
    },
    zIndex: {
        justifyContent: 'space-around',
        width: 100,
        height: 50,
        marginTop: -10,
    },
    blankStyle: {},
    border: {
        borderRadius: 5,
    },
});
var FlowDirectionChange = /** @class */ (function (_super) {
    __extends(FlowDirectionChange, _super);
    function FlowDirectionChange() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            useBorder: false,
        };
        _this.handleClick = function () {
            _this.setState({ useBorder: !_this.state.useBorder });
        };
        return _this;
    }
    FlowDirectionChange.prototype.render = function () {
        var currentStyle = this.state.useBorder
            ? styles.withBorder
            : styles.withoutBorder;
        return (React.createElement(react_native_1.View, { style: { flex: 1 } },
            React.createElement(react_native_1.View, { style: [styles.withoutBorder, currentStyle] },
                React.createElement(react_native_1.Text, { style: { fontSize: 11 } }, "This text should remain on the right after changing the radius")),
            React.createElement(react_native_1.Button, { onPress: this.handleClick, title: "Change Border Radius" })));
    };
    return FlowDirectionChange;
}(React.Component));
var ZIndexChange = /** @class */ (function (_super) {
    __extends(ZIndexChange, _super);
    function ZIndexChange() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            cornerRadius: false,
            flipped: false,
        };
        _this.handleClick = function () {
            _this.setState({ cornerRadius: !_this.state.cornerRadius });
        };
        _this.handlePress = function () {
            _this.setState({ flipped: !_this.state.flipped });
        };
        return _this;
    }
    ZIndexChange.prototype.render = function () {
        var hasRadius = this.state.cornerRadius
            ? styles.border
            : styles.blankStyle;
        var indices = this.state.flipped ? [-1, 0, 1, 2] : [2, 1, 0, -1];
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.View, null,
                React.createElement(react_native_1.Text, { style: { paddingBottom: 10 } }, "The zIndex should remain the same after changing the border radius"),
                React.createElement(react_native_1.View, { testID: "automationID", style: [
                        styles.zIndex,
                        {
                            marginTop: 0,
                            backgroundColor: '#E57373',
                            zIndex: indices[0],
                        },
                        hasRadius,
                    ] },
                    React.createElement(react_native_1.Text, null,
                        "ZIndex ",
                        indices[0])),
                React.createElement(react_native_1.View, { style: [
                        styles.zIndex,
                        {
                            marginLeft: 50,
                            backgroundColor: '#FFF176',
                            zIndex: indices[1],
                        },
                        hasRadius,
                    ] },
                    React.createElement(react_native_1.Text, null,
                        "ZIndex ",
                        indices[1])),
                React.createElement(react_native_1.View, { style: [
                        styles.zIndex,
                        {
                            marginLeft: 100,
                            backgroundColor: '#81C784',
                            zIndex: indices[2],
                        },
                        hasRadius,
                    ] },
                    React.createElement(react_native_1.Text, null,
                        "ZIndex ",
                        indices[2])),
                React.createElement(react_native_1.View, { style: [
                        styles.zIndex,
                        {
                            marginLeft: 150,
                            backgroundColor: '#64B5F6',
                            zIndex: indices[3],
                        },
                        hasRadius,
                    ] },
                    React.createElement(react_native_1.Text, null,
                        "ZIndex ",
                        indices[3]))),
            React.createElement(react_native_1.Button, { onPress: this.handleClick, title: "Change Border Radius" }),
            React.createElement(react_native_1.Button, { onPress: this.handlePress, title: "Change Sorting Order" })));
    };
    return ZIndexChange;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = '<TransferProperties>';
exports.description = 'Some tests that change the backing XAML element ' +
    'to see if transfer properties is working.';
exports.examples = [
    {
        title: 'Flow Direction Change',
        render: function () {
            return React.createElement(FlowDirectionChange, null);
        },
    },
    {
        title: 'zIndex Change',
        render: function () {
            return React.createElement(ZIndexChange, null);
        },
    },
];
//# sourceMappingURL=TransferPropertiesExample.js.map