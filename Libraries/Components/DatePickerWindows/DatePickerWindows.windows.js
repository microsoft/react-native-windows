/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule DatePickerWindows
 * @flow
 */
'use strict';

var React = require('React');
var View = require('View');
var requireNativeComponent = require('requireNativeComponent');

var ReactNative = require("react-native");

var DatePickerWindows = React.createClass({
    name: 'DatePickerWindows',
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
         * date and time.
         */
        onChange: React.PropTypes.func,

        /**
         * Maximum year.
         *
         * Restricts the range with an upper bound on the year.
         * Works only in 'date', 'time' and 'datetime' modes.
         */
        maxYear: React.PropTypes.instanceOf(Date),

        /**
         * Minimum year.
         *
         * Restricts the range with an lower bound on the year.
         * Works only in 'date', 'time' and 'datetime' modes.
         */
        minYear: React.PropTypes.instanceOf(Date),

        /**
         * Minimum date
         * Restricts the range with an lower bound.
         * Works only in the 'calendar' mode.
         */
        minDate: React.PropTypes.instanceOf(Date),

        /**
         * Maximum date
         * Restricts the range with an upper bound.
         * Works only in the 'calendar' mode.
         */
        maxDate: React.PropTypes.instanceOf(Date),

        /**
         * The date picker mode.
         * Modes:
         * - time: Time selector
         * - date: Date selector, in a scrollable interface
         * - datetime: date and time modes combined
         * - calendar: Date selector using a calendar view
         */
        mode: React.PropTypes.oneOf(['date', 'time', 'datetime', 'calendar']),
    },
    getDefaultProps: function() {
      return {
        mode: 'datetime',
        date: new Date(),
      };
    },
    _onDateChange: function(event) {
      this.props.onDateChange && this.props.onDateChange(new Date(event.nativeEvent.date));
    },
    renderPicker: function() {
      switch(this.props.mode){
        case 'date':
          return <RCTDatePicker 
                    date={this.props.date}
                    minYear={this.props.minYear}
                    maxYear={this.props.maxYear}
                    onChange={this._onDateChange} />
          case 'time':
            return <RCTTimePicker
                      date={this.props.date}
                      onChange={this._onDateChange} />
          case 'calendar':
            return <RCTCalendarDatePicker
                    date={this.props.date}
                    minDate={this.props.minDate}
                    maxDate={this.props.maxDate}
                    onChange={this._onDateChange} />
          default:
            throw new Error("Invalid mode for DatePickerWindows. Options are 'date', 'time', 'datetime', 'calendar'")
      }
    },
    render: function() {
      return this.renderPicker();
    }
})

var RCTDatePicker = requireNativeComponent('RCTDatePicker', DatePickerWindows);
var RCTTimePicker = requireNativeComponent('RCTTimePicker', DatePickerWindows);
var RCTCalendarDatePicker = requireNativeComponent('RCTCalendarDatePicker', DatePickerWindows);

module.exports = DatePickerWindows;