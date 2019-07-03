/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import * as RN from 'react-native';

export enum EventPhase {
  None = 0,
  Capturing,
  AtTarget,
  Bubbling,
}

export enum HandledEventPhase {
  Capturing = EventPhase.Capturing,
  Bubbling = EventPhase.Bubbling,
}

export interface INativeKeyboardEvent {
  altKey: boolean;
  ctrlKey: boolean;
  metaKey: boolean;
  shiftKey: boolean;
  key: string;
  eventPhase: EventPhase;
}

export interface IHandledKeyboardEvent {
  altKey?: boolean;
  ctrlKey?: boolean;
  metaKey?: boolean;
  shiftKey?: boolean;
  key: string;
  handledEventPhase?: HandledEventPhase;
}

export type IKeyboardEvent = RN.NativeSyntheticEvent<INativeKeyboardEvent>;

export interface IKeyboardProps {
  onKeyDown?: (args: IKeyboardEvent) => void;
  onKeyDownCapture?: (args: IKeyboardEvent) => void;
  onKeyUp?: (args: IKeyboardEvent) => void;
  onKeyUpCapture?: (args: IKeyboardEvent) => void;

  keyDownEvents?: IHandledKeyboardEvent[];
  keyUpEvents?: IHandledKeyboardEvent[];
}
