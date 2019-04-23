import * as React from 'react';
import { IPickerItemProps, IPickerProps } from './PickerProps';
declare class PickerItem extends React.Component<IPickerItemProps> {
    render(): JSX.Element | null;
}
export interface IPickerItemData {
    label: string;
    value?: any;
    textColor?: number;
}
interface State {
    selectedIndex: number;
    items: IPickerItemData[];
}
declare type PickerPropsWithChildren = Readonly<{
    children?: React.ReactNode;
}> & Readonly<IPickerProps>;
/**
 * Picker is a controlled component, which expects the selectedValue prop to be updated
 * whenever selection changes, or selection will revert to the prop selectedValue
 *
 * when using editable=true, onValueChange can be called with a selectedValue of null &
 * Index of -1, and text will be provided.
 * To maintain the text in the controlled component, props should reflect
 * that state by specifying selectedValue of null and specify the text property.
 */
export declare class Picker extends React.Component<IPickerProps, State> {
    static Item: typeof PickerItem;
    private _rctPicker;
    static getDerivedStateFromProps(props: PickerPropsWithChildren): State;
    constructor(props: IPickerProps);
    render(): JSX.Element;
    private _setRef;
    private _onChange;
}
export default Picker;
