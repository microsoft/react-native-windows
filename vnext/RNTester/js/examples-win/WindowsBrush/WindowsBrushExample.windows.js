"use strict";
/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
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
var React = require("react");
var react_native_1 = require("react-native");
var WindowsBrushExample = /** @class */ (function (_super) {
    __extends(WindowsBrushExample, _super);
    function WindowsBrushExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this._onPress = function () { };
        return _this;
    }
    WindowsBrushExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text
            // @ts-ignore
            , { 
                // @ts-ignore
                style: { color: { windowsbrush: 'SystemAccentColorLight3' } } }, "Sample Text"),
            React.createElement(react_native_1.Button, { title: "Sample Button", 
                // @ts-ignore
                color: { windowsbrush: 'SystemAccentColorLight3' }, onPress: this._onPress })));
    };
    return WindowsBrushExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = 'WindowsBrush';
exports.description = 'Usage of windowsbrush for color props';
exports.examples = [
    {
        title: 'WindowsBrush control',
        render: function () {
            return React.createElement(WindowsBrushExample, null);
        },
    },
];
//# sourceMappingURL=WindowsBrushExample.windows.js.map