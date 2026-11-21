/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<54af040912510a2a03a017d91145562b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\LogBox\Data\LogBoxLog.js
 */

import type { Stack } from "./LogBoxSymbolication";
import type { Category, CodeFrame, Message } from "./parseLogBoxLog";
type SymbolicationStatus = "NONE" | "PENDING" | "COMPLETE" | "FAILED";
type SymbolicationState = Readonly<{
  error: null;
  stack: null;
  status: "NONE";
}> | Readonly<{
  error: null;
  stack: null;
  status: "PENDING";
}> | Readonly<{
  error: null;
  stack: Stack;
  status: "COMPLETE";
}> | Readonly<{
  error: Error;
  stack: null;
  status: "FAILED";
}>;
export type LogLevel = "warn" | "error" | "fatal" | "syntax";
export type LogBoxLogData = Readonly<{
  level: LogLevel;
  type?: string | undefined;
  message: Message;
  stack: Stack;
  category: string;
  componentStack: Stack;
  codeFrame?: CodeFrame | undefined;
  isComponentError: boolean;
  extraData?: unknown | undefined;
  onNotificationPress?: (() => void) | undefined;
}>;
declare class LogBoxLog {
  message: Message;
  type: null | undefined | string;
  category: Category;
  componentStack: Stack;
  stack: Stack;
  count: number;
  level: LogLevel;
  codeFrame: null | undefined | CodeFrame;
  componentCodeFrame: null | undefined | CodeFrame;
  isComponentError: boolean;
  extraData: unknown | void;
  symbolicated: SymbolicationState;
  symbolicatedComponentStack: SymbolicationState;
  onNotificationPress: null | undefined | (() => void);
  constructor(data: LogBoxLogData);
  incrementCount(): void;
  getAvailableStack(): Stack;
  getAvailableComponentStack(): Stack;
  retrySymbolicate(callback?: (status: SymbolicationStatus) => void): void;
  symbolicate(callback?: (status: SymbolicationStatus) => void): void;
  handleSymbolicate(callback?: (status: SymbolicationStatus) => void): void;
  updateStatus(error: null | undefined | Error, stack: null | undefined | Stack, codeFrame: null | undefined | CodeFrame, callback?: (status: SymbolicationStatus) => void): void;
  updateComponentStackStatus(error: null | undefined | Error, stack: null | undefined | Stack, codeFrame: null | undefined | CodeFrame, callback?: (status: SymbolicationStatus) => void): void;
}
export default LogBoxLog;
