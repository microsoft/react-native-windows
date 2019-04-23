import * as React from 'react';
import { IPopupProps } from './PopupProps';
export interface IPopupTargetState {
    target?: number | null;
    targetRef?: React.ReactNode;
}
/**
 * Renders a popup component.
 *
 * This is a controlled component that requires an `onDismiss` callback that
 * updates the `isOpen` prop in order for the component to reflect user actions.
 *
 * @keyword popup
 */
export declare class Popup extends React.Component<IPopupProps, IPopupTargetState> {
    static getDerivedStateFromProps(nextProps: IPopupProps, prevState: IPopupTargetState): IPopupTargetState;
    constructor(props: IPopupProps);
    render(): JSX.Element;
}
export default Popup;
