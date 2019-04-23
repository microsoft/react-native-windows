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
var react_native_1 = require("react-native");
var React = require("react");
var styles = react_native_1.StyleSheet.create({
    rctDatePicker: {
        height: 32,
        width: 150,
    },
});
var RCTDatePicker = react_native_1.requireNativeComponent('RCTDatePicker');
var DatePicker = /** @class */ (function (_super) {
    __extends(DatePicker, _super);
    function DatePicker(props) {
        var _this = _super.call(this, props) || this;
        _this._setRef = function (datepicker /*RCTDatePicker*/) {
            _this._rctDatePicker = datepicker;
        };
        _this._onChange = function (event) {
            if (_this.props.selectedDate) {
                var propsTimeStamp = _this.props.selectedDate.getTime();
                if (_this._rctDatePicker) {
                    _this._rctDatePicker.setNativeProps({ selectedDate: propsTimeStamp });
                }
            }
            // Change the props after the native props are set in case the props
            // change removes the component
            _this.props.onChange && _this.props.onChange(event);
            var nativeTimeStamp = event.nativeEvent.newDate;
            _this.props.onDateChange &&
                _this.props.onDateChange(new Date(+nativeTimeStamp)); // Added the '+' operator to convert string to number
        };
        _this._rctDatePicker = React.createRef();
        return _this;
    }
    DatePicker.prototype.render = function () {
        var props = {
            dayOfWeekFormat: this.props.dayOfWeekFormat,
            dateFormat: this.props.dateFormat,
            firstDayOfWeek: this.props.firstDayOfWeek,
            maxDate: (this.props.maxDate ? (this.props.maxDate.getTime()) : undefined),
            minDate: (this.props.minDate ? (this.props.minDate.getTime()) : undefined),
            onChange: this.props.onChange,
            placeholderText: this.props.placeholderText,
            selectedDate: (this.props.selectedDate ? (this.props.selectedDate.getTime()) : undefined),
            style: [styles.rctDatePicker, this.props.style]
        };
        // The Date object returns timezone in minutes. Convert that to seconds
        // and multiply by -1 so that the offset can be added to GMT time to get
        // the correct value on the native side.
        var timeZoneOffsetInSeconds = this.props.timeZoneOffsetInSeconds
            ? this.props.timeZoneOffsetInSeconds
            : this.props.selectedDate
                ? (-1 * this.props.selectedDate.getTimezoneOffset() * 60)
                : undefined;
        return (React.createElement(RCTDatePicker, __assign({}, props, { onChange: this._onChange, ref: this._setRef, timeZoneOffsetInSeconds: timeZoneOffsetInSeconds })));
    };
    DatePicker.defaultProps = {
        dateFormat: 'dayofweek day month'
    };
    return DatePicker;
}(React.Component));
exports.DatePicker = DatePicker;
exports.default = DatePicker;
//# sourceMappingURL=DatePicker.uwp.js.map