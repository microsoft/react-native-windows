/**
 * @providesModule TimePickerWindows
 * @flow
 */
'use strict';

var PropTypes = require('prop-types');
var React = require('React');
var ReactNative = require('ReactNative');
var UIManager = require('UIManager');
var ViewPropTypes = require('ViewPropTypes');

var requireNativeComponent = require('requireNativeComponent');

var TIMEPICKER_REF = 'timePicker';

/**
 * React component that wraps the Windows-only `TimePicker`.
 */
class TimePickerWindows extends React.Component {
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
     * date and time.
     */
    onChange: PropTypes.func,
  };

  static defaultProps = {
    date: new Date(),
  }

  _onChange = (event) => {
    this.props.onChange && this.props.onChange(new Date(event.nativeEvent.date));
  }

  render(){
    return <NativeWindowsTimePickerWindows
            ref={TIMEPICKER_REF}
            date={this.props.date}
            onChange={this._onChange} />
  }
}

var NativeWindowsTimePickerWindows = requireNativeComponent('TimePickerWindows', TimePickerWindows);

module.exports = TimePickerWindows;
