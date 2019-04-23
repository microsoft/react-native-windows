import * as React from 'react';
interface IRNTesterNavigationState {
    openExample?: string;
}
interface IRNTesterAppProps {
    exampleFromAppetizeParams: string;
}
declare class RNTesterApp extends React.Component<IRNTesterAppProps, IRNTesterNavigationState> {
    render(): JSX.Element;
    componentDidMount(): void;
    private _handleBack;
    private _handleAction;
}
export = RNTesterApp;
