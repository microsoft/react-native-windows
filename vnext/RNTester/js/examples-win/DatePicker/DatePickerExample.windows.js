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
var DatePicker_1 = require("../../../../Libraries/Components/DatePicker/DatePicker");
var DatePickerProps_1 = require("../../../../Libraries/Components/DatePicker/DatePickerProps");
var PickerWindows_1 = require("../../../../Libraries/Components/Picker/PickerWindows");
var DatePickerExample = /** @class */ (function (_super) {
    __extends(DatePickerExample, _super);
    function DatePickerExample() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.state = {
            dateFormat: 'longdate',
            dayOfWeekFormat: '{dayofweek.abbreviated(2)}',
            selectedDate: new Date(),
            firstDayOfWeek: DatePickerProps_1.DayOfWeek.Sunday,
            maxDate: new Date('2050'),
            minDate: new Date('1950'),
        };
        _this.onDateChange = function (newDate) {
            _this.setState({ selectedDate: newDate });
        };
        return _this;
    }
    DatePickerExample.prototype.render = function () {
        var _this = this;
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { style: { padding: 10 } },
                "Please choose from the settings below:",
                ' '),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "Date format: "),
                React.createElement(PickerWindows_1.Picker, { style: { width: 200, height: 35 }, selectedValue: this.state.dateFormat, onValueChange: function (value) { return _this.setState({ dateFormat: value }); } },
                    React.createElement(PickerWindows_1.Picker.Item, { label: "day month year", value: "day month year" }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "dayofweek day month", value: "dayofweek day month" }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "longdate", value: "longdate" }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "shortdate", value: "shortdate" }))),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "Day Of week format: "),
                React.createElement(PickerWindows_1.Picker, { style: { width: 200, height: 35 }, selectedValue: this.state.dayOfWeekFormat, onValueChange: function (value) { return _this.setState({ dayOfWeekFormat: value }); } },
                    React.createElement(PickerWindows_1.Picker.Item, { label: "abbreviated(2)", value: "{dayofweek.abbreviated(2)}" }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "abbreviated(3)", value: "{dayofweek.abbreviated(3)}" }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "full", value: "{dayofweek.full}" }))),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "First day Of week: "),
                React.createElement(PickerWindows_1.Picker, { style: { width: 200, height: 35 }, selectedValue: this.state.firstDayOfWeek, onValueChange: function (value) { return _this.setState({ firstDayOfWeek: value }); } },
                    React.createElement(PickerWindows_1.Picker.Item, { label: "Sunday", value: DatePickerProps_1.DayOfWeek.Sunday }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "Monday", value: DatePickerProps_1.DayOfWeek.Monday }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "Tuesday", value: DatePickerProps_1.DayOfWeek.Tuesday }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "Wednesday", value: DatePickerProps_1.DayOfWeek.Wednesday }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "Thursday", value: DatePickerProps_1.DayOfWeek.Thursday }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "Friday", value: DatePickerProps_1.DayOfWeek.Friday }),
                    React.createElement(PickerWindows_1.Picker.Item, { label: "Saturday", value: DatePickerProps_1.DayOfWeek.Saturday }))),
            React.createElement(DatePicker_1.DatePicker, { style: { paddingTop: 10, width: 350 }, selectedDate: this.state.selectedDate, placeholderText: "select date", dateFormat: this.state.dateFormat, dayOfWeekFormat: this.state.dayOfWeekFormat, firstDayOfWeek: this.state.firstDayOfWeek, onDateChange: this.onDateChange, maxDate: this.state.maxDate, minDate: this.state.minDate }),
            React.createElement(react_native_1.Text, null,
                "Date Changed event response: ",
                this.state.selectedDate.toString()),
            React.createElement(react_native_1.Text, null,
                "Maximum date: ",
                this.state.maxDate.toDateString()),
            React.createElement(react_native_1.Text, null,
                "Minimum date: ",
                this.state.minDate.toDateString())));
    };
    return DatePickerExample;
}(React.Component));
exports.displayName = function (_undefined) { };
exports.title = '<DatePicker>';
exports.description = 'Component that allows a user to pick a date from a calendar display with UWP CalendarDatePicker.';
exports.examples = [
    {
        title: '<DatePicker>',
        render: function () {
            return React.createElement(DatePickerExample, null);
        },
    },
];
//# sourceMappingURL=DatePickerExample.windows.js.map