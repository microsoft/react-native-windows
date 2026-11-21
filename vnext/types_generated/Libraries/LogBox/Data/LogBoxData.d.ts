/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<602ded3c7666ba42ed31efe90b36826b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\LogBox\Data\LogBoxData.js
 */

import type { ExtendedError } from "../../Core/ExtendedError";
import type { LogLevel } from "./LogBoxLog";
import type { Stack } from "./LogBoxSymbolication";
import type { Category, ExtendedExceptionData, Message } from "./parseLogBoxLog";
import LogBoxLog from "./LogBoxLog";
import * as React from "react";
export type LogBoxLogs = Set<LogBoxLog>;
export type LogData = Readonly<{
  level: LogLevel;
  message: Message;
  category: Category;
  componentStack: Stack;
  stack?: string | undefined;
}>;
export type Observer = ($$PARAM_0$$: Readonly<{
  logs: LogBoxLogs;
  isDisabled: boolean;
  selectedLogIndex: number;
}>) => void;
export type IgnorePattern = string | RegExp;
export type Subscription = Readonly<{
  unsubscribe: () => void;
}>;
export type WarningInfo = {
  finalFormat: string;
  forceDialogImmediately: boolean;
  suppressDialog_LEGACY: boolean;
  suppressCompletely: boolean;
  monitorEvent: string | null;
  monitorListVersion: number;
  monitorSampleRate: number;
};
export type WarningFilter = (format: string) => WarningInfo;
type AppInfo = Readonly<{
  appVersion: string;
  engine: string;
  onPress?: (() => void) | undefined;
}>;
declare let logs: LogBoxLogs;
export declare function reportLogBoxError(error: ExtendedError, componentStack?: string): void;
export declare function isLogBoxErrorMessage(message: string): boolean;
export declare function isMessageIgnored(message: string): boolean;
export declare function addLog(log: LogData): void;
export declare function addException(error: ExtendedExceptionData): void;
export declare function symbolicateLogNow(log: LogBoxLog): void;
export declare function retrySymbolicateLogNow(log: LogBoxLog): void;
export declare function symbolicateLogLazy(log: LogBoxLog): void;
export declare function clear(): void;
export declare function setSelectedLog(proposedNewIndex: number): void;
export declare function clearWarnings(): void;
export declare function clearErrors(): void;
export declare function dismiss(log: LogBoxLog): void;
export declare function setWarningFilter(filter: WarningFilter): void;
export declare function setAppInfo(info: () => AppInfo): void;
export declare function getAppInfo(): null | undefined | AppInfo;
export declare function checkWarningFilter(format: string): WarningInfo;
export declare function getIgnorePatterns(): ReadonlyArray<IgnorePattern>;
export declare function addIgnorePatterns(patterns: ReadonlyArray<IgnorePattern>): void;
export declare function setDisabled(value: boolean): void;
export declare function isDisabled(): boolean;
export declare function observe(observer: Observer): Subscription;
type SubscribedComponent = React.ComponentType<Readonly<{
  logs: ReadonlyArray<LogBoxLog>;
  isDisabled: boolean;
  selectedLogIndex: number;
}>>;
export declare function withSubscription(WrappedComponent: SubscribedComponent): React.ComponentType<{}>;
