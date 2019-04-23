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
    static title: string;
    static description: string;
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
export default TextExample;
