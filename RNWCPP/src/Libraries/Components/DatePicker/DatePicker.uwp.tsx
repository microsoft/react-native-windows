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
    dateFormat: 'dayofweek day month'
  };

  // tslint:disable-next-line:no-any
  private _rctDatePicker: any;

  public constructor(props: IDatePickerProps) {
    super(props);
    this._rctDatePicker = React.createRef();
  }
  public render(): JSX.Element {
    const props = { ...this.props };
    props.style = [styles.rctDatePicker, this.props.style];

    return (
      <RCTDatePicker
        { ...props }
        onChange={ this._onChange }
        ref={ this._setRef }
      />
    );
  }

  private _setRef = (datepicker: DatePicker/*RCTDatePicker*/) => {
    this._rctDatePicker = datepicker;
  }

  private _onChange = (event: IDatePickerChangeEvent) => {
    if (this._rctDatePicker) {
      this._rctDatePicker.setNativeProps({ selectedDate: this.props.selectedDate });
    }

    // Change the props after the native props are set in case the props
    // change removes the component
    this.props.onChange && this.props.onChange(event);
    this.props.onDateChange &&
      this.props.onDateChange(event.nativeEvent.newDate);
  }
}

export default DatePicker;
