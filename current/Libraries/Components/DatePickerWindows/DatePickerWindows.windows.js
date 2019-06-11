/**
 * @providesModule DatePickerWindows
 * @flow
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ViewPropTypes = require('ViewPropTypes');

var requireNativeComponent = require('requireNativeComponent');

var DATEPICKER_REF = 'datePicker';

/**
 * React component that wraps the Windows-only `DatePicker`.
 */
class DatePickerWindows extends React.Component {
  props: {
    date?: Date;
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
     * Minimum year.
     *
     * Restricts the range with an lower bound on the year.
     */
    minYear: PropTypes.instanceOf(Date),

    /**
     * Maximum year.
     *
     * Restricts the range with an upper bound on the year.
     */
    maxYear: PropTypes.instanceOf(Date),
  };

  static defaultProps = {
    date: new Date(),
  }

  _onChange = (event) => {
    this.props.onChange && this.props.onChange(new Date(event.nativeEvent.date));
  }

  render(){
    let {date, minYear, maxYear} = this.props;
    date = date.getTime();
    minYear = minYear ? minYear.getTime() : undefined;
    maxYear = maxYear ? maxYear.getTime() : undefined;

    return <NativeWindowsDatePicker
              ref={DATEPICKER_REF}
              style={this.props.style}
              date={date}
              minYear={minYear}
              maxYear={maxYear}
              onChange={this._onChange}
            />
  }

}

var NativeWindowsDatePicker = requireNativeComponent('DatePickerWindows', DatePickerWindows);

module.exports = DatePickerWindows;
