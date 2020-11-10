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
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
var __rest = (this && this.__rest) || function (s, e) {
    var t = {};
    for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p) && e.indexOf(p) < 0)
        t[p] = s[p];
    if (s != null && typeof Object.getOwnPropertySymbols === "function")
        for (var i = 0, p = Object.getOwnPropertySymbols(s); i < p.length; i++) {
            if (e.indexOf(p[i]) < 0 && Object.prototype.propertyIsEnumerable.call(s, p[i]))
                t[p[i]] = s[p[i]];
        }
    return t;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.title = exports.displayName = exports.CustomFrameworkElement = void 0;
var React = require("react");
var react_native_1 = require("react-native");
// JS to wrap the custom element.
// Typically you would export this class as the reusable component
var RCTCustomFrameworkElement = react_native_1.requireNativeComponent('RCTCustomFrameworkElement');
var CustomFrameworkElement = /** @class */ (function (_super) {
    __extends(CustomFrameworkElement, _super);
    function CustomFrameworkElement() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.styles = react_native_1.StyleSheet.create({
            default: {
                height: 40,
                width: 40,
            },
        });
        _this._onCustomEvent = function (event) {
            _this.props.onCustomEvent && _this.props.onCustomEvent(event);
        };
        return _this;
    }
    CustomFrameworkElement.prototype.render = function () {
        // eslint-disable-next-line @typescript-eslint/no-unused-vars
        var _a = this.props, onCustomEvent = _a.onCustomEvent, rest = __rest(_a, ["onCustomEvent"]);
        rest.style = [this.styles.default, this.props.style];
        return (React.createElement(RCTCustomFrameworkElement, __assign({}, rest, { onCustomEvent: this._onCustomEvent })));
    };
    CustomFrameworkElement.prototype.exampleCommand = function () {
        react_native_1.NativeModules.UIManager.dispatchViewManagerCommand(react_native_1.findNodeHandle(this), react_native_1.NativeModules.UIManager.RCTCustomFrameworkElement.Commands.ExampleCommand, null);
    };
    return CustomFrameworkElement;
}(React.Component));
exports.CustomFrameworkElement = CustomFrameworkElement;
// Example consuming the component
var CustomViewExample = /** @class */ (function (_super) {
    __extends(CustomViewExample, _super);
    function CustomViewExample(props) {
        var _this = _super.call(this, props) || this;
        _this.state = { countOfEvents: 0 };
        _this.onCustomEvent = function (_event) {
            _this.setState({ countOfEvents: _this.state.countOfEvents + 1 });
        };
        _this.onPress = function () {
            if (_this._customElementRef) {
                _this._customElementRef.exampleCommand();
            }
        };
        _this._setRef = function (customElement) {
            _this._customElementRef = customElement;
        };
        _this._customElementRef = React.createRef();
        return _this;
    }
    CustomViewExample.prototype.render = function () {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, null, "A simple RCTCustomFrameworkElement:"),
            React.createElement(CustomFrameworkElement, { disabled: false, test: true, onCustomEvent: this.onCustomEvent, ref: this._setRef }),
            React.createElement(react_native_1.Text, null,
                "count of events: ",
                this.state.countOfEvents),
            React.createElement(react_native_1.Button, { title: "Run test command", onPress: this.onPress })));
    };
    return CustomViewExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = 'Custom Views';
exports.description = 'Example of custom view managers.';
exports.examples = [
    {
        title: 'Simple',
        render: function () {
            return React.createElement(CustomViewExample, null);
        },
    },
];
//# sourceMappingURL=CustomViewExample.windows.js.map