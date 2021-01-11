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
var AppTheme_1 = require("../../../../Libraries/AppTheme/AppTheme");
var ThemeExample = /** @class */ (function (_super) {
    __extends(ThemeExample, _super);
    function ThemeExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            currentTheme: AppTheme_1.AppTheme.currentTheme,
        };
        _this.onAppThemeChanged = function (event) {
            var currentTheme = event.currentTheme;
            _this.setState({ currentTheme: currentTheme });
        };
        _this._onPress = function () { };
        return _this;
    }
    ThemeExample.prototype.componentDidMount = function () {
        AppTheme_1.AppTheme.addListener('appThemeChanged', this.onAppThemeChanged);
    };
    ThemeExample.prototype.componentWillUnmount = function () {
        AppTheme_1.AppTheme.removeListener('appThemeChanged', this.onAppThemeChanged);
    };
    ThemeExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { style: { color: 'red' } },
                "currentTheme: ",
                this.state.currentTheme),
            React.createElement(react_native_1.Button, { onPress: this._onPress, title: this.state.currentTheme, color: this.state.currentTheme === 'dark' ? 'grey' : 'orange' })));
    };
    return ThemeExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = 'AppTheme';
exports.description = 'Usage of theme properties.';
exports.examples = [
    {
        title: 'Theme Aware Control',
        render: function () {
            return React.createElement(ThemeExample, null);
        },
    },
];
//# sourceMappingURL=ThemingExample.windows.js.map