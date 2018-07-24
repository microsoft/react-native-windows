/**
 * @providesModule CalendarDatePickerWindows
 * @flow
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ReactNative = require('ReactNative');
var UIManager = require('UIManager');
var ViewPropTypes = require('ViewPropTypes');

var requireNativeComponent = require('requireNativeComponent');

var CALENDARPICKER_REF = 'calendarPicker';

/**
 * React component that wraps the Windows-only `CalendarDatePicker`.
 */
class CalendarDatePickerWindows extends React.Component {
  props: {
    firstDayOfWeek?: number;
    calendarIdentifier?: string;
  }
  static propTypes = {
    ...ViewPropTypes,
    /**
     * The currently selected date.
     */
    date: PropTypes.instanceOf(Date),
    /**
     * Date change handler.
     *
     * This is called when the user changes the date or time in the UI.
     * The first and only argument is a Date object representing the new
     * date.
     */
    onChange: PropTypes.func,

    /**
     * Minimum date.
     * 
     * Restricts the range with an lower bound.
     */
    minDate: PropTypes.instanceOf(Date),

    /**
     * Maximum date.
     * 
     * Restricts the range with an upper bound.
     */
    maxDate: PropTypes.instanceOf(Date),

    /**
     * Placeholder text for the component.
     */
    placeholderText: PropTypes.string,

     /**
     * Calendar type, defined as a string.
     * 
     * The supported values are defined in the Windows.Globalization.CalendarIdentifiers class.
     * Some values require API v10.0.14393.0 or above
     */
    calendarIdentifier: PropTypes.string,
    /**
     * First day of the week, defined as an integer with 0 being Sunday, 1 Monday, etc.
     */
    firstDayOfWeek: PropTypes.number,
  };

  static defaultProps = {
    date: new Date(),
  }

  _onChange = (event) => {
    this.props.onChange && this.props.onChange(new Date(event.nativeEvent.date));
  }

  render(){
    let {date, minDate, maxDate} = this.props;
    date = date.getTime();
    minDate = minDate ? minDate.getTime() : undefined;
    maxDate = maxDate ? maxDate.getTime() : undefined;

    return <NativeWindowsCalendarDatePicker
              ref={CALENDARPICKER_REF}
              style={this.props.style}
              date={date}
              minDate={minDate}
              maxDate={maxDate}
              onChange={this._onChange}
              placeholderText={this.props.placeholderText}
              calendarIdentifier={this.props.calendarIdentifier}
              firstDayOfWeek={this.props.firstDayOfWeek} 
            />
  }

}

var NativeWindowsCalendarDatePicker = requireNativeComponent('CalendarDatePickerWindows', CalendarDatePickerWindows);

module.exports = CalendarDatePickerWindows;
