/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import * as React from 'react';
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
export declare const displayName: (_undefined?: string | undefined) => void;
export declare const title = "Custom Views";
export declare const description = "Example of custom view managers.";
export declare const examples: {
    title: string;
    render: () => JSX.Element;
}[];
//# sourceMappingURL=CustomViewExample.windows.d.ts.map