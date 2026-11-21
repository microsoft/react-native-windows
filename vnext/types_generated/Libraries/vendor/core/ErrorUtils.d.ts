/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<1bfecae9d7794571133121e93812fd77>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\vendor\core\ErrorUtils.js
 */

type ErrorHandler = (error: unknown, isFatal: boolean) => void;
type Fn<Args extends ReadonlyArray<unknown>, Return> = (...$$REST$$: Args) => Return;
export type ErrorUtils = {
  applyWithGuard<TArgs extends ReadonlyArray<unknown>, TOut>(fun: Fn<TArgs, TOut>, context?: unknown, args?: TArgs | undefined, unused_onError?: null, unused_name?: string | undefined): TOut | undefined;
  applyWithGuardIfNeeded<TArgs extends ReadonlyArray<unknown>, TOut>(fun: Fn<TArgs, TOut>, context?: unknown, args?: TArgs | undefined): TOut | undefined;
  getGlobalHandler(): ErrorHandler;
  guard<TArgs extends ReadonlyArray<unknown>, TOut>(fun: Fn<TArgs, TOut>, name?: string | undefined, context?: unknown): ((...$$REST$$: TArgs) => TOut | undefined) | undefined;
  inGuard(): boolean;
  reportError(error: unknown): void;
  reportFatalError(error: unknown): void;
  setGlobalHandler(fun: ErrorHandler): void;
};
/**
 * The particular require runtime that we are using looks for a global
 * `ErrorUtils` object and if it exists, then it requires modules with the
 * error handler specified via ErrorUtils.setGlobalHandler by calling the
 * require function with applyWithGuard. Since the require module is loaded
 * before any of the modules, this ErrorUtils must be defined (and the handler
 * set) globally before requiring anything.
 *
 * However, we still want to treat ErrorUtils as a module so that other modules
 * that use it aren't just using a global variable, so simply export the global
 * variable here. ErrorUtils is originally defined in a file named error-guard.js.
 */
declare const $$ErrorUtils: ErrorUtils;
declare type $$ErrorUtils = typeof $$ErrorUtils;
export default $$ErrorUtils;
