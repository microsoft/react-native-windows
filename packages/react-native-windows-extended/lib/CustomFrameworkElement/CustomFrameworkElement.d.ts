/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import React = require('react');
import { ViewProps } from 'react-native';
export interface ICustomFrameworkElementProps extends ViewProps {
    disabled?: boolean;
    test: boolean;
    onCustomEvent?: (event: ICustomChangeEvent) => void;
}
export interface ICustomChangeEvent {
    nativeEvent: {
        value: boolean;
    };
}
export declare class CustomFrameworkElement extends React.Component<ICustomFrameworkElementProps, {}> {
    private styles;
    render(): JSX.Element;
    exampleCommand(): void;
    private _onCustomEvent;
}
