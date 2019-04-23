import * as React from 'react';
import { ICheckBoxProps } from './CheckBoxProps';
/**
 * Renders a boolean input.
 *
 * This is a controlled component that requires an `onValueChange` callback that
 * updates the `checked` prop in order for the component to reflect user actions.
 * If the `checked` prop is not updated, the component will continue to render
 * the supplied `checked` prop instead of the expected result of any user actions.
 *
 * @keyword checkbox
 * @keyword toggle
 */
export declare class CheckBox extends React.Component<ICheckBoxProps> {
    private _rctCheckBox;
    constructor(props: ICheckBoxProps);
    render(): JSX.Element;
    private _setRef;
    private _onChange;
}
export default CheckBox;
