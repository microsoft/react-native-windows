/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Text, View} from 'react-native';
import {DatePicker, Picker, DayOfWeek} from '../index.uwp';

interface IDatePickerExampleState {
  dateFormat:
    | 'day month year'
    | 'dayofweek day month'
    | 'longdate'
    | 'shortdate';
  dayOfWeekFormat:
    | '{dayofweek.abbreviated(2)}'
    | '{dayofweek.abbreviated(3)}'
    | '{dayofweek.full}';
  selectedDate: Date;
  firstDayOfWeek: DayOfWeek;
  maxDate: Date;
  minDate: Date;
}

class DatePickerExample extends React.Component<{}, IDatePickerExampleState> {
  public state: IDatePickerExampleState = {
    dateFormat: 'longdate',
    dayOfWeekFormat: '{dayofweek.abbreviated(2)}',
    selectedDate: new Date(),
    firstDayOfWeek: DayOfWeek.Sunday,
    maxDate: new Date('2050'),
    minDate: new Date('1950'),
  };

  public render() {
    return (
      <View>
        <Text style={{padding: 10}}>
          Please choose from the settings below:{' '}
        </Text>
        <View style={{flexDirection: 'row'}}>
          <Text style={{padding: 10}}>Date format: </Text>
          <Picker
            style={{width: 200, height: 35}}
            selectedValue={this.state.dateFormat}
            onValueChange={value => this.setState({dateFormat: value})}>
            <Picker.Item label="day month year" value="day month year" />
            <Picker.Item
              label="dayofweek day month"
              value="dayofweek day month"
            />
            <Picker.Item label="longdate" value="longdate" />
            <Picker.Item label="shortdate" value="shortdate" />
          </Picker>
        </View>
        <View style={{flexDirection: 'row'}}>
          <Text style={{padding: 10}}>Day Of week format: </Text>
          <Picker
            style={{width: 200, height: 35}}
            selectedValue={this.state.dayOfWeekFormat}
            onValueChange={value => this.setState({dayOfWeekFormat: value})}>
            <Picker.Item
              label="abbreviated(2)"
              value="{dayofweek.abbreviated(2)}"
            />
            <Picker.Item
              label="abbreviated(3)"
              value="{dayofweek.abbreviated(3)}"
            />
            <Picker.Item label="full" value="{dayofweek.full}" />
          </Picker>
        </View>
        <View style={{flexDirection: 'row'}}>
          <Text style={{padding: 10}}>First day Of week: </Text>
          <Picker
            style={{width: 200, height: 35}}
            selectedValue={this.state.firstDayOfWeek}
            onValueChange={value => this.setState({firstDayOfWeek: value})}>
            <Picker.Item label="Sunday" value={DayOfWeek.Sunday} />
            <Picker.Item label="Monday" value={DayOfWeek.Monday} />
            <Picker.Item label="Tuesday" value={DayOfWeek.Tuesday} />
            <Picker.Item label="Wednesday" value={DayOfWeek.Wednesday} />
            <Picker.Item label="Thursday" value={DayOfWeek.Thursday} />
            <Picker.Item label="Friday" value={DayOfWeek.Friday} />
            <Picker.Item label="Saturday" value={DayOfWeek.Saturday} />
          </Picker>
        </View>
        <DatePicker
          style={{paddingTop: 10, width: 350}}
          selectedDate={this.state.selectedDate}
          placeholderText="select date"
          dateFormat={this.state.dateFormat}
          dayOfWeekFormat={this.state.dayOfWeekFormat}
          firstDayOfWeek={this.state.firstDayOfWeek}
          onDateChange={this.onDateChange}
          maxDate={this.state.maxDate}
          minDate={this.state.minDate}
        />
        <Text>
          Date Changed event response: {this.state.selectedDate.toString()}
        </Text>
        <Text>Maximum date: {this.state.maxDate.toDateString()}</Text>
        <Text>Minimum date: {this.state.minDate.toDateString()}</Text>
      </View>
    );
  }

  onDateChange = (newDate: Date) => {
    this.setState({selectedDate: newDate});
  };
}

export const displayName = (_undefined?: string) => {};
export const title = '<DatePicker>';
export const description =
  'Component that allows a user to pick a date from a calendar display with uwp CalendarDatePicker.';
export const examples = [
  {
    title: '<DatePicker>',
    render: function(): JSX.Element {
      return <DatePickerExample />;
    },
  },
];
