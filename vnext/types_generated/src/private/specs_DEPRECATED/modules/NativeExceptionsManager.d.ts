/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<00ed5cd00eae3e8ba18b313ff15361ce>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\specs_DEPRECATED\modules\NativeExceptionsManager.js
 */

import type { TurboModule } from "../../../../Libraries/TurboModule/RCTExport";
export type StackFrame = {
  column: number | undefined;
  file: string | undefined;
  lineNumber: number | undefined;
  methodName: string;
  collapse?: boolean | undefined;
};
export type ExceptionData = {
  message: string;
  originalMessage: string | undefined;
  name: string | undefined;
  componentStack: string | undefined;
  stack: Array<StackFrame>;
  id: number;
  isFatal: boolean;
  extraData?: Object | undefined;
};
export interface Spec extends TurboModule {
  readonly reportFatalException: (message: string, stack: Array<StackFrame>, exceptionId: number) => void;
  readonly reportSoftException: (message: string, stack: Array<StackFrame>, exceptionId: number) => void;
  readonly reportException?: ((data: ExceptionData) => void) | undefined;
  readonly dismissRedbox?: (() => void) | undefined;
}
declare const ExceptionsManager: {
  reportFatalException(message: string, stack: Array<StackFrame>, exceptionId: number): void;
  reportSoftException(message: string, stack: Array<StackFrame>, exceptionId: number): void;
  dismissRedbox(): void;
  reportException(data: ExceptionData): void;
};
declare const $$NativeExceptionsManager: typeof ExceptionsManager;
declare type $$NativeExceptionsManager = typeof $$NativeExceptionsManager;
export default $$NativeExceptionsManager;
