// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';
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
Object.defineProperty(exports, "__esModule", { value: true });
var React = require("react");
var react_native_1 = require("react-native");
var styles = react_native_1.StyleSheet.create({
    rctCheckBox: {
        height: 32,
        width: 32,
    },
});
var RCTCheckBox = react_native_1.requireNativeComponent('RCTCheckBox');
/**
 * Renders a boolean input.
 *
 * This is a controlled component that requires an `onValueChange` callback that
 * updates the `checked` prop in order for the component to reflect user actions.
 * If the `checked` prop is not updated, the component will continue to render
 * the supplied `checked` prop instead of the expected result of any user actions.
 *
 * @keyword checkbox
 * @keyword toggle
 */
var CheckBox = /** @class */ (function (_super) {
    __extends(CheckBox, _super);
    function CheckBox(props) {
        var _this = _super.call(this, props) || this;
        _this._setRef = function (checkBox /*RCTCheckBox*/) {
            _this._rctCheckBox = checkBox;
        };
        _this._onChange = function (event) {
            if (_this._rctCheckBox) {
                _this._rctCheckBox.setNativeProps({ checked: _this.props.checked });
            }
            // Change the props after the native props are set in case the props
            // change removes the component
            _this.props.onChange && _this.props.onChange(event);
            _this.props.onValueChange &&
                _this.props.onValueChange(event.nativeEvent.value);
        };
        _this._rctCheckBox = React.createRef();
        return _this;
    }
    CheckBox.prototype.render = function () {
        var props = __assign({}, this.props);
        props.onStartShouldSetResponder = function () { return true; };
        props.onResponderTerminationRequest = function () { return false; };
        props.style = [styles.rctCheckBox, this.props.style];
        return (React.createElement(RCTCheckBox, __assign({}, props, { onChange: this._onChange, ref: this._setRef })));
    };
    return CheckBox;
}(React.Component));
exports.CheckBox = CheckBox;
exports.default = CheckBox;
//# sourceMappingURL=CheckBox.uwp.js.map