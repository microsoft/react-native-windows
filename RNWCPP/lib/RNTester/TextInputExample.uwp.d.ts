import React = require('react');
import ReactNative = require('react-native');
interface IAutogrowingTextInputExampleProps {
    multiline: boolean;
    enablesReturnKeyAutomatically: boolean;
    returnKeyType: ReactNative.ReturnKeyTypeOptions;
}
export declare class AutogrowingTextInputExample extends React.Component<any, any> {
    constructor(props: IAutogrowingTextInputExampleProps);
    UNSAFE_componentWillReceiveProps(props: IAutogrowingTextInputExampleProps): void;
    render(): JSX.Element;
}
export declare const displayName: undefined;
export declare const title = "<TextInput>";
export declare const description = "Single and multi-line text inputs.";
export declare const examples: {
    title: string;
    render: () => React.ReactNode;
}[];
export {};
