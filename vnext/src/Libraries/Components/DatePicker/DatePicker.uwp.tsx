// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import { requireNativeComponent, StyleSheet } from 'react-native';
import { IDatePickerProps, IDatePickerChangeEvent } from './DatePickerProps';
import * as React from 'react';

const styles = StyleSheet.create({
  rctDatePicker: {
    height: 32,
    width: 150,
  },
});

const RCTDatePicker = requireNativeComponent('RCTDatePicker');

export class DatePicker extends React.Component<IDatePickerProps> {

  public static defaultProps: IDatePickerProps = {
    dateFormat: 'dayofweek day month',
  };

  // tslint:disable-next-line:no-any
  private _rctDatePicker: any;

  public constructor(props: IDatePickerProps) {
    super(props);
    this._rctDatePicker = React.createRef();
  }
  public render(): JSX.Element {
    const props = {
      dayOfWeekFormat: this.props.dayOfWeekFormat,
      dateFormat: this.props.dateFormat,
      firstDayOfWeek: this.props.firstDayOfWeek,
      maxDate: (this.props.maxDate ? (this.props.maxDate.getTime()) : undefined), // time in milliseconds
      minDate: (this.props.minDate ? (this.props.minDate.getTime()) : undefined), // time in milliseconds
      onChange: this.props.onChange,
      placeholderText: this.props.placeholderText,
      selectedDate: (this.props.selectedDate ? (this.props.selectedDate.getTime()) : undefined), // time in milliseconds
      style: [styles.rctDatePicker, this.props.style],
    };

    // The Date object returns timezone in minutes. Convert that to seconds
    // and multiply by -1 so that the offset can be added to GMT time to get
    // the correct value on the native side.
    const timeZoneOffsetInSeconds =
      this.props.timeZoneOffsetInSeconds
        ? this.props.timeZoneOffsetInSeconds
        : this.props.selectedDate
          ? (-1 * this.props.selectedDate.getTimezoneOffset() * 60)
          : undefined;

    return (
      <RCTDatePicker
        { ...props }
        onChange={ this._onChange }
        ref={ this._setRef }
        timeZoneOffsetInSeconds={ timeZoneOffsetInSeconds }
      />
    );
  }

  private _setRef = (datepicker: DatePicker/*RCTDatePicker*/) => {
    this._rctDatePicker = datepicker;
  }

  private _onChange = (event: IDatePickerChangeEvent) => {
    if (this.props.selectedDate) {
      const propsTimeStamp = this.props.selectedDate.getTime();
      if (this._rctDatePicker) {
        this._rctDatePicker.setNativeProps({ selectedDate: propsTimeStamp });
      }
    }

    // Change the props after the native props are set in case the props
    // change removes the component
    this.props.onChange && this.props.onChange(event);

    const nativeTimeStamp = event.nativeEvent.newDate;
    this.props.onDateChange &&
      this.props.onDateChange(new Date(+nativeTimeStamp)); // Added the '+' operator to convert string to number
  }
}

export default DatePicker;
