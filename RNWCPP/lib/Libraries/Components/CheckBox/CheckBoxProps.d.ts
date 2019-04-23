import { ViewProps } from 'react-native';
export interface ICheckBoxProps extends ViewProps {
    checked?: boolean;
    disabled?: boolean;
    onChange?: (event: ICheckBoxChangeEvent) => void;
    onValueChange?: (value: boolean) => void;
}
export interface ICheckBoxChangeEvent {
    nativeEvent: {
        value: boolean;
    };
}
