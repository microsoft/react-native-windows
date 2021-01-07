/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import * as RN from 'react-native';

// Enum already part of public API
// eslint-disable-next-line no-restricted-syntax
export enum EventPhase {
  None = 0,
  Capturing,
  AtTarget,
  Bubbling,
}

// Enum already part of public API
// eslint-disable-next-line no-restricted-syntax
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
  code: string;
  eventPhase: EventPhase;
}

export interface IHandledKeyboardEvent {
  altKey?: boolean;
  ctrlKey?: boolean;
  metaKey?: boolean;
  shiftKey?: boolean;
  code: string;
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
