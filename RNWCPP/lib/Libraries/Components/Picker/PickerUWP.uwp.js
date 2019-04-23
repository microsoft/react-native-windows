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
var RCTPicker = react_native_1.requireNativeComponent('RCTPicker');
var PickerItem = /** @class */ (function (_super) {
    __extends(PickerItem, _super);
    function PickerItem() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    PickerItem.prototype.render = function () {
        return null;
    };
    return PickerItem;
}(React.Component));
/**
 * Picker is a controlled component, which expects the selectedValue prop to be updated
 * whenever selection changes, or selection will revert to the prop selectedValue
 *
 * when using editable=true, onValueChange can be called with a selectedValue of null &
 * Index of -1, and text will be provided.
 * To maintain the text in the controlled component, props should reflect
 * that state by specifying selectedValue of null and specify the text property.
 */
var Picker = /** @class */ (function (_super) {
    __extends(Picker, _super);
    function Picker(props) {
        var _this = _super.call(this, props) || this;
        _this._setRef = function (comboBox /*RCTPicker*/) {
            _this._rctPicker = comboBox;
        };
        _this._onChange = function (event) {
            if (_this._rctPicker) {
                _this._rctPicker.setNativeProps({ selectedIndex: _this.state.selectedIndex, text: _this.props.text });
            }
            _this.props.onChange && _this.props.onChange(event);
            _this.props.onValueChange &&
                _this.props.onValueChange(event.nativeEvent.value, event.nativeEvent.itemIndex, event.nativeEvent.text);
        };
        _this._rctPicker = React.createRef();
        _this.state = {
            selectedIndex: 0,
            items: [],
        };
        return _this;
    }
    Picker.getDerivedStateFromProps = function (props) {
        var selectedIndex = -1;
        var items = [];
        React.Children.toArray(props.children).forEach(function (c, index) {
            var child = c;
            if (child.props.value === props.selectedValue) {
                selectedIndex = index;
            }
            items.push({
                value: child.props.value,
                label: child.props.label,
                textColor: react_native_1.processColor(child.props.color),
            });
        });
        return { selectedIndex: selectedIndex, items: items };
    };
    Picker.prototype.render = function () {
        var props = __assign({}, this.props);
        props.onStartShouldSetResponder = function () { return true; };
        props.onResponderTerminationRequest = function () { return false; };
        props.style = this.props.style;
        return (React.createElement(RCTPicker, __assign({}, props, { items: this.state.items, selectedIndex: this.state.selectedIndex, onChange: this._onChange, ref: this._setRef }), this.props.children));
    };
    Picker.Item = PickerItem;
    return Picker;
}(React.Component));
exports.Picker = Picker;
exports.default = Picker;
//# sourceMappingURL=PickerUWP.uwp.js.map