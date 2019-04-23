import * as React from 'react';
import { IPickerItemProps, IPickerProps } from './PickerProps';
declare class PickerItem extends React.Component<IPickerItemProps> {
    render(): JSX.Element | null;
}
export declare class Picker extends React.Component<IPickerProps> {
    static Item: typeof PickerItem;
    render(): JSX.Element | null;
}
export default Picker;
