import { NativeSyntheticEvent, TextInputFocusEventData } from 'react-native';
interface IChangePayload {
    eventCount: number;
    target: number;
    text: string;
}
export declare type IChangeEvent = NativeSyntheticEvent<Readonly<IChangePayload>>;
interface IRange {
    start: number;
    end: number;
}
interface ITextInputPayload {
    eventCount: number;
    previousText: string;
    range: Readonly<IRange>;
    target: number;
    text: number;
}
export declare type ITextInputevent = NativeSyntheticEvent<Readonly<ITextInputPayload>>;
interface IContentSize {
    width: number;
    height: number;
}
interface IContentSizeChangePayload {
    target: number;
    contentSize: Readonly<IContentSize>;
}
export declare type IContentSizeChangeEvent = NativeSyntheticEvent<Readonly<IContentSizeChangePayload>>;
export declare type IBlurEvent = NativeSyntheticEvent<Readonly<TextInputFocusEventData>>;
export declare type IFocusEvent = NativeSyntheticEvent<Readonly<TextInputFocusEventData>>;
export interface ISelection extends IRange {
}
interface ISelectionPayload {
    selection: ISelection;
    target: number;
}
export declare type ISelectionChangeEvent = NativeSyntheticEvent<Readonly<ISelectionPayload>>;
interface IKeyPressPayload {
    key: string;
    target?: number;
    eventCount?: number;
}
export declare type IKeyPressEvent = NativeSyntheticEvent<Readonly<IKeyPressPayload>>;
interface IEditingPayload {
    eventCount: number;
    text: string;
    target: number;
}
export declare type IEditingEvent = NativeSyntheticEvent<Readonly<IEditingPayload>>;
export declare type IPasswordRules = string;
export {};
