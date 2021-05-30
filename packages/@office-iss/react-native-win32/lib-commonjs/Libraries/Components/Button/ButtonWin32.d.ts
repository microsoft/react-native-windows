import React from 'react';
import RN = require('react-native');
import { IButtonWin32Props } from './ButtonWin32.Props';
interface IButtonWin32State {
    accessibilityState: RN.AccessibilityState;
}
/**
 * React-native <Button> control with additional Win32-specific functionality.
 */
export declare class ButtonWin32 extends React.Component<IButtonWin32Props, IButtonWin32State> {
    constructor(props: IButtonWin32Props);
    render(): JSX.Element;
    private readonly _makeState;
    private readonly _setState;
    private readonly _onFocus;
    private readonly _onBlur;
    private readonly _onTouchEnd;
}
export {};
