/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule CalendarPickerWindows
 * @flow
 */
'use strict';

var React = require('React');
var View = require('View');
var requireNativeComponent = require('requireNativeComponent');

var ReactNative = require("react-native");

var CalendarPickerWindows = React.createClass({
    name: 'CalendarPickerWindows',
    propTypes: {
        ...View.PropTypes,
        ...ReactNative.ViewPropTypes,

        /**
         * The currently selected date.
         */
        date: React.PropTypes.instanceOf(Date),
        /**
         * Date change handler.
         *
         * This is called when the user changes the date or time in the UI.
         * The first and only argument is a Date object representing the new
         * date.
         */
        onChange: React.PropTypes.func,

        /**
         * Minimum date.
         * 
         * Restricts the range with an lower bound.
         */
        minDate: React.PropTypes.instanceOf(Date),

        /**
         * Maximum date.
         * 
         * Restricts the range with an upper bound.
         */
        maxDate: React.PropTypes.instanceOf(Date),

        /**
         * Placeholder text for the component.
         */
        placeholderText: React.PropTypes.string,

         /**
         * Calendar type, defined as a string.
         * 
         * The supported values are defined in the Windows.Globalization.CalendarIdentifiers class.
         * Some values require API v10.0.14393.0 or above
         */
        calendarIdentifier: React.PropTypes.oneOf('ChineseLunarCalendar', 'GregorianCalendar', 'HebrewCalendar',
                                                  'HijriCalendar', 'JapaneseCalendar', 'JapaneseLunarCalendar',
                                                  'JulianCalendar', 'KoreanCalendar', 'KoreanLunarCalendar',
                                                  'PersianCalendar', 'TaiwanCalendar', 'TaiwanLunarCalendar',
                                                  'ThaiCalendar', 'UmAlQuraCalendar', 'VietnameseLunarCalendar'),
        /**
         * First day of the week, defined as an integer with 0 being Sunday, 1 Monday, etc.
         */
        firstDayOfWeek: React.PropTypes.oneOf(0, 1, 2, 3, 4, 5, 6),
    },
    getDefaultProps: function() {
      return {
        firstDayOfWeek: 0,
        calendarIdentifier: 'GregorianCalendar',
      };
    },
    _onChange: function(event) {
      this.props.onChange && this.props.onChange(new Date(event.nativeEvent.date));
    },
    render: function() {
      return <RCTCalendarDatePicker
                  style={this.props.style}
                  date={this.props.date}
                  minDate={this.props.minDate}
                  maxDate={this.props.maxDate}
                  onChange={this._onChange}
                  placeholderText={this.props.placeholderText}
                  calendarIdentifier={this.props.calendarIdentifier}
                  firstDayOfWeek={this.props.firstDayOfWeek} />
    },
});

var RCTCalendarDatePicker = requireNativeComponent('RCTCalendarDatePicker', CalendarPickerWindows);

module.exports = CalendarPickerWindows;
