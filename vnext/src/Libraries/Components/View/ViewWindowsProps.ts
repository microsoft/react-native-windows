// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as RN  from 'react-native';

// removes from interface T the members of interface K
type Omit<T, K> = Pick<T, Exclude<keyof T, keyof K>>;

// All of T becomes optional except for Keys
type PartiallyRequired<T, Keys extends keyof T = keyof T> = Pick<Partial<T>, Exclude<keyof T, Keys>> & Pick<T, Keys>;

export enum EventPhase {
  None = 0,
  Capturing,
  AtTarget,
  Bubbling
}

export interface INativeKeyboardEvent {
  altKey: boolean;
  ctrlKey: boolean;
  metaKey: boolean;
  shiftKey: boolean;
  key: string;
  eventPhase: EventPhase;
}

export type IKeyboardEvent = RN.NativeSyntheticEvent<INativeKeyboardEvent>;
export type IHandledKeyboardEvent = PartiallyRequired<INativeKeyboardEvent, 'key'>;

export interface IViewWindows {

}

export type ViewWindowsOmitTypes = RN.ViewPropsAndroid &
  RN.ViewPropsIOS &
  RN.AccessibilityPropsAndroid &
  RN.AccessibilityPropsIOS;

/**
 * Properties for ViewWindows component
 */
export interface IViewWindowsProps extends Omit<RN.ViewProps, ViewWindowsOmitTypes> {
  onKeyDown?: (args: IKeyboardEvent) => void;
  onKeyDownCapture?: (args: IKeyboardEvent) => void;
  onKeyUp?: (args: IKeyboardEvent) => void;
  onKeyUpCapture?: (args: IKeyboardEvent) => void;

  keyDownEvents?: IHandledKeyboardEvent[];
  keyUpEvents?: IHandledKeyboardEvent[];
}
