// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as React from 'react';
import { processColor, requireNativeComponent, StyleSheet } from 'react-native';
import { ICalendarViewProps, ICalendarViewChangeEvent } from './CalendarViewProps';

const styles = StyleSheet.create({
  rctCalendarView: {
    height: 340,
    width: 280,
  },
});

const RCTCalendarView = requireNativeComponent('RCTCalendarView');

/**
 * Renders a calendar view component.
 *
 * @keyword calendarview
 * @keyword minicalendar
 */
export class CalendarView extends React.Component<ICalendarViewProps, {}> {

  // tslint:disable-next-line:no-any
  private _rctCalendarView: any;

  public constructor(props: ICalendarViewProps) {
    super(props);
    this._rctCalendarView = React.createRef();
  }

  public render(): JSX.Element {
    // Note: we don't use const props = { ...this.props } here because we need to convert string? color properties
    // from ICalendarViewProps to number ? color properties(as needed on C++ side) - via processColor calls.
    // For the sake of consistency, please keep properties below in alphabetical order (on property names).
    const props = {
      calendarIdentifier: this.props.calendarIdentifier,
      calendarItemBackgroundColor: processColor(this.props.calendarItemBackgroundColor),
      calendarItemBorderColor: processColor(this.props.calendarItemBorderColor),
      calendarItemBorderThickness: this.props.calendarItemBorderThickness,
      calendarItemColor: processColor(this.props.calendarItemColor),
      dayItemFontFamily: this.props.dayItemFontFamily,
      dayItemFontSize: this.props.dayItemFontSize,
      dayItemFontWeight: this.props.dayItemFontWeight,
      dayOfWeekFormat: this.props.dayOfWeekFormat,
      firstDayOfWeek: this.props.firstDayOfWeek,
      firstOfMonthLabelFontFamily: this.props.firstOfMonthLabelFontFamily,
      firstOfMonthLabelFontSize: this.props.firstOfMonthLabelFontSize,
      firstOfMonthLabelFontWeight: this.props.firstOfMonthLabelFontWeight,
      firstOfYearDecadeLabelFontFamily: this.props.firstOfYearDecadeLabelFontFamily,
      firstOfYearDecadeLabelFontSize: this.props.firstOfYearDecadeLabelFontSize,
      firstOfYearDecadeLabelFontWeight: this.props.firstOfYearDecadeLabelFontWeight,
      focusBorderColor: processColor(this.props.focusBorderColor),
      horizontalDayItemAlignment: this.props.horizontalDayItemAlignment,
      horizontalFirstOfMonthLabelAlignment: this.props.horizontalFirstOfMonthLabelAlignment,
      hoverBorderColor: processColor(this.props.hoverBorderColor),
      isGroupLabelVisible: this.props.isGroupLabelVisible,
      isOutOfScopeEnabled: this.props.isOutOfScopeEnabled,
      isTodayHighlighted: this.props.isTodayHighlighted,
      maxDate: (this.props.maxDate ? this.props.maxDate.getTime() : undefined),  // time in milliseconds
      minDate: (this.props.minDate ? this.props.minDate.getTime() : undefined),  // time in milliseconds
      monthYearItemFontFamily: this.props.monthYearItemFontFamily,
      monthYearItemFontSize: this.props.monthYearItemFontSize,
      monthYearItemFontWeight: this.props.monthYearItemFontWeight,
      numberOfWeeksInView: this.props.numberOfWeeksInView,
      onChange: this.props.onChange,
      onSelectedDateChange: this.props.onSelectedDateChange,
      outOfScopeColor: processColor(this.props.outOfScopeColor),
      outOfScopeBackgroundColor: processColor(this.props.outOfScopeBackgroundColor),
      pressedBorderColor: processColor(this.props.pressedBorderColor),
      pressedColor: processColor(this.props.pressedColor),
      selectedBorderColor: processColor(this.props.selectedBorderColor),
      selectedColor: processColor(this.props.selectedColor),
      selectedDate: (this.props.selectedDate ? this.props.selectedDate.getTime() : undefined),  // time in milliseconds
      selectedHoverBorderColor: processColor(this.props.selectedHoverBorderColor),
      selectedPressedBorderColor: processColor(this.props.selectedPressedBorderColor),
      style: [styles.rctCalendarView, this.props.style],
      todayColor: processColor(this.props.todayColor),
      todayFontWeight: this.props.todayFontWeight,
      verticalDayItemAlignment: this.props.verticalDayItemAlignment,
      verticalFirstOfMonthLabelAlignment: this.props.verticalFirstOfMonthLabelAlignment,
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
      <RCTCalendarView
        { ...props }
        onChange={ this._onChange }
        ref={ this._setRef }
        timeZoneOffsetInSeconds={ timeZoneOffsetInSeconds }
      />
    );
  }

  private _setRef = (datepicker: CalendarView/*RCTCalendarView*/) => {
    this._rctCalendarView = datepicker;
  }

  private _onChange = (event: ICalendarViewChangeEvent) => {
    if (this.props.selectedDate) {
      const propsTimeStamp = this.props.selectedDate.getTime();
      if (this._rctCalendarView) {
        this._rctCalendarView.setNativeProps({ selectedDate: propsTimeStamp });
      }
    }

    // Change the props after the native props are set in case the props
    // change removes the component
    this.props.onChange && this.props.onChange(event);

    const nativeTimeStamp = event.nativeEvent.selectedDate;
    this.props.onSelectedDateChange &&
      this.props.onSelectedDateChange(new Date(+nativeTimeStamp)); // Added the '+' operator to convert string to number
  }
}

export default CalendarView;
