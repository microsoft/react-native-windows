/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import {ViewProps} from 'react-native';

export interface IDatePickerProps extends ViewProps {
  selectedDate?: Date;
  placeholderText?: string;
  dateFormat?:
    | 'day month year'
    | 'dayofweek day month'
    | 'longdate'
    | 'shortdate';
  dayOfWeekFormat?:
    | '{dayofweek.abbreviated(2)}'
    | '{dayofweek.abbreviated(3)}'
    | '{dayofweek.full}';
  firstDayOfWeek?: DayOfWeek;
  maxDate?: Date;
  minDate?: Date;
  timeZoneOffsetInSeconds?: number;
  onChange?: (event: IDatePickerChangeEvent) => void;
  onDateChange?: (newDate: Date) => void;
}

export interface IDatePickerChangeEvent {
  nativeEvent: {
    newDate: number;
  };
}

export enum DayOfWeek {
  Sunday = 0,
  Monday = 1,
  Tuesday = 2,
  Wednesday = 3,
  Thursday = 4,
  Friday = 5,
  Saturday = 6,
}
