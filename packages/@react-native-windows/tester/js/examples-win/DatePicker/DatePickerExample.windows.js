"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.examples = exports.description = exports.category = exports.title = exports.displayName = void 0;
const React = require("react");
const react_native_1 = require("react-native");
const react_native_windows_1 = require("react-native-windows");
class DatePickerExample extends React.Component {
    constructor() {
        super(...arguments);
        this.state = {
            dateFormat: 'longdate',
            dayOfWeekFormat: '{dayofweek.abbreviated(2)}',
            selectedDate: new Date(),
            firstDayOfWeek: react_native_windows_1.DayOfWeek.Sunday,
            maxDate: new Date('2050'),
            minDate: new Date('1950'),
        };
        this.onDateChange = (newDate) => {
            this.setState({ selectedDate: newDate });
        };
    }
    render() {
        return (React.createElement(react_native_1.View, null,
            React.createElement(react_native_1.Text, { style: { padding: 10 } },
                "Please choose from the settings below:",
                ' '),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "Date format: "),
                React.createElement(react_native_windows_1.Picker, { style: { width: 200, height: 35 }, selectedValue: this.state.dateFormat, onValueChange: value => this.setState({ dateFormat: value }) },
                    React.createElement(react_native_windows_1.Picker.Item, { label: "day month year", value: "day month year" }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "dayofweek day month", value: "dayofweek day month" }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "longdate", value: "longdate" }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "shortdate", value: "shortdate" }))),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "Day Of week format: "),
                React.createElement(react_native_windows_1.Picker, { style: { width: 200, height: 35 }, selectedValue: this.state.dayOfWeekFormat, onValueChange: value => this.setState({ dayOfWeekFormat: value }) },
                    React.createElement(react_native_windows_1.Picker.Item, { label: "abbreviated(2)", value: "{dayofweek.abbreviated(2)}" }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "abbreviated(3)", value: "{dayofweek.abbreviated(3)}" }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "full", value: "{dayofweek.full}" }))),
            React.createElement(react_native_1.View, { style: { flexDirection: 'row' } },
                React.createElement(react_native_1.Text, { style: { padding: 10 } }, "First day Of week: "),
                React.createElement(react_native_windows_1.Picker, { style: { width: 200, height: 35 }, selectedValue: this.state.firstDayOfWeek, onValueChange: value => this.setState({ firstDayOfWeek: value }) },
                    React.createElement(react_native_windows_1.Picker.Item, { label: "Sunday", value: react_native_windows_1.DayOfWeek.Sunday }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "Monday", value: react_native_windows_1.DayOfWeek.Monday }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "Tuesday", value: react_native_windows_1.DayOfWeek.Tuesday }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "Wednesday", value: react_native_windows_1.DayOfWeek.Wednesday }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "Thursday", value: react_native_windows_1.DayOfWeek.Thursday }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "Friday", value: react_native_windows_1.DayOfWeek.Friday }),
                    React.createElement(react_native_windows_1.Picker.Item, { label: "Saturday", value: react_native_windows_1.DayOfWeek.Saturday }))),
            React.createElement(react_native_windows_1.DatePicker, { style: { paddingTop: 10, width: 350 }, selectedDate: this.state.selectedDate, placeholderText: "select date", dateFormat: this.state.dateFormat, dayOfWeekFormat: this.state.dayOfWeekFormat, firstDayOfWeek: this.state.firstDayOfWeek, onDateChange: this.onDateChange, maxDate: this.state.maxDate, minDate: this.state.minDate }),
            React.createElement(react_native_1.Text, null,
                "Date Changed event response: ",
                this.state.selectedDate.toString()),
            React.createElement(react_native_1.Text, null,
                "Maximum date: ",
                this.state.maxDate.toDateString()),
            React.createElement(react_native_1.Text, null,
                "Minimum date: ",
                this.state.minDate.toDateString())));
    }
}
exports.displayName = (_undefined) => { };
exports.title = 'DatePicker';
exports.category = 'UI';
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