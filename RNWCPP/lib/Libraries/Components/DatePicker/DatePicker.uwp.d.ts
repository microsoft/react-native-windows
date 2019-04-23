import { IDatePickerProps } from './DatePickerProps';
import * as React from 'react';
export declare class DatePicker extends React.Component<IDatePickerProps> {
    static defaultProps: IDatePickerProps;
    private _rctDatePicker;
    constructor(props: IDatePickerProps);
    render(): JSX.Element;
    private _setRef;
    private _onChange;
}
export default DatePicker;
