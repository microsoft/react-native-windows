// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

import { ViewProps } from 'react-native';

export interface ICalendarViewProps extends ViewProps {
  // For the sake of consistency, please keep declarations below in alphabetical order (on property names).
  calendarIdentifier?: string;
  calendarItemBackgroundColor?: string;
  calendarItemBorderColor?: string;
  calendarItemBorderThickness?: number;
  calendarItemColor?: string;
  dayItemFontFamily?: string;
  dayItemFontSize?: number;
  dayItemFontWeight?: number;
  dayOfWeekFormat?: '{dayofweek.abbreviated(2)}' | '{dayofweek.abbreviated(3)}' | '{dayofweek.full}';
  firstDayOfWeek?: 0 | 1 | 2 | 3 | 4 | 5 | 6;
  firstOfMonthLabelFontFamily?: string;
  firstOfMonthLabelFontSize?: number;
  firstOfMonthLabelFontWeight?: number;
  firstOfYearDecadeLabelFontFamily?: string;
  firstOfYearDecadeLabelFontSize?: number;
  firstOfYearDecadeLabelFontWeight?: number;
  focusBorderColor?: string;
  horizontalDayItemAlignment?: 'center' | 'left' | 'right' | 'stretch';
  horizontalFirstOfMonthLabelAlignment?: 'center' | 'left' | 'right' | 'stretch';
  hoverBorderColor?: string;
  isGroupLabelVisible?: boolean;
  isOutOfScopeEnabled?: boolean;
  isTodayHighlighted?: boolean;
  maxDate?: Date;
  minDate?: Date;
  monthYearItemFontFamily?: string;
  monthYearItemFontSize?: number;
  monthYearItemFontWeight?: number;
  numberOfWeeksInView?: 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10;
  onChange?: (event: ICalendarViewChangeEvent) => void;
  onSelectedDateChange?: (selectedDate: Date) => void;
  outOfScopeColor?: string;
  outOfScopeBackgroundColor?: string;
  pressedBorderColor?: string;
  pressedColor?: string;
  selectedBorderColor?: string;
  selectedColor?: string;
  selectedDate?: Date;
  selectedHoverBorderColor?: string;
  selectedPressedBorderColor?: string;
  todayColor?: string;
  todayFontWeight?: number;
  verticalDayItemAlignment?: 'bottom' | 'center' | 'stretch' | 'top';
  verticalFirstOfMonthLabelAlignment?: 'bottom' | 'center' | 'stretch' | 'top';
}

export interface ICalendarViewChangeEvent {
  nativeEvent: {
    selectedDate: Date;
  };
}
