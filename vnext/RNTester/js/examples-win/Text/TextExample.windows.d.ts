/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
import React = require('react');
export declare class Entity extends React.Component<{}> {
    render(): JSX.Element;
}
interface IAttributeTogglerState {
    fontWeight: 'bold' | 'normal';
    fontSize: number;
}
export declare class AttributeToggler extends React.Component<{}, IAttributeTogglerState> {
    state: IAttributeTogglerState;
    toggleWeight: () => void;
    increaseSize: () => void;
    render(): JSX.Element;
}
export declare class TextExample extends React.Component<{}> {
    render(): JSX.Element;
}
export declare const styles: {
    backgroundColorText: {
        left: number;
        backgroundColor: string;
    };
    includeFontPaddingText: {
        fontSize: number;
        fontFamily: string;
        backgroundColor: string;
        color: string;
        textAlignVertical: "center";
        alignSelf: "center";
    };
};
export declare const displayName: (_undefined?: string | undefined) => void;
export declare const title = "<Text>";
export declare const description = "Base component for rendering styled text.";
export declare const examples: {
    render: () => JSX.Element;
}[];
export {};
//# sourceMappingURL=TextExample.windows.d.ts.map