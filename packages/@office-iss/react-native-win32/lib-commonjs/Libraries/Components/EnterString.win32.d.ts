import React from 'react';
export interface IEnterStringProps {
    disabled?: boolean;
    value?: string;
    label?: string;
    onChanged?: (newValue: string) => void;
}
export default class EnterString extends React.Component<IEnterStringProps, {}> {
    static DefaultProps: IEnterStringProps;
    render(): JSX.Element;
    private _onChangedNative;
}
