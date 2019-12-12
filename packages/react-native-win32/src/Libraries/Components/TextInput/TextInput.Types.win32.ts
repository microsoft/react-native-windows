import { NativeSyntheticEvent, TextInputFocusEventData } from 'react-native';

// TODO: How do we keep in sync with synthetic events

interface IChangePayload {
  eventCount: number;
  target: number;
  text: string;
}
export type IChangeEvent = NativeSyntheticEvent<Readonly<IChangePayload>>;

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
export type ITextInputevent = NativeSyntheticEvent<Readonly<ITextInputPayload>>;

interface IContentSize {
  width: number;
  height: number;
}
interface IContentSizeChangePayload {
  target: number;
  contentSize: Readonly<IContentSize>;
}
export type IContentSizeChangeEvent = NativeSyntheticEvent<Readonly<IContentSizeChangePayload>>;

// TODO: Why doesn't this guy work
// interface ITargetPayload {
//   target: number;
// }

// type ITargetEvent = NativeSyntheticEvent<Readonly<ITargetPayload>>;

export type IBlurEvent = NativeSyntheticEvent<Readonly<TextInputFocusEventData>>;
export type IFocusEvent = NativeSyntheticEvent<Readonly<TextInputFocusEventData>>;

export interface ISelection extends IRange { }
interface ISelectionPayload {
  selection: ISelection;
  target: number;
}
export type ISelectionChangeEvent = NativeSyntheticEvent<Readonly<ISelectionPayload>>;

// TODO: Why is this one diffferent?
interface IKeyPressPayload {
  key: string;
  target?: number;
  eventCount?: number;
}
export type IKeyPressEvent = NativeSyntheticEvent<Readonly<IKeyPressPayload>>;

interface IEditingPayload {
  eventCount: number;
  text: string;
  target: number;
}
export type IEditingEvent = NativeSyntheticEvent<Readonly<IEditingPayload>>;

// TODO: Why do I need this one
export type IPasswordRules = string;

// TODO: Can I get away without this?
export interface IRCTInput {
  blur(): void;
  focus(): void;
  clear(): void;
}
