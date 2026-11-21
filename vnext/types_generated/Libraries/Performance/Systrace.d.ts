/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<87f12127d457bf47dee101ef610680f8>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Performance\Systrace.js
 */

type EventName = string | (() => string);
type EventArgs = null | undefined | {
  [$$Key$$: string]: string;
};
/**
 * Indicates if the application is currently being traced.
 *
 * Calling methods on this module when the application isn't being traced is
 * cheap, but this method can be used to avoid computing expensive values for
 * those functions.
 *
 * @example
 * if (Systrace.isEnabled()) {
 *   const expensiveArgs = computeExpensiveArgs();
 *   Systrace.beginEvent('myEvent', expensiveArgs);
 * }
 */
export declare function isEnabled(): boolean;
/**
 * @deprecated This function is now a no-op but it's left for backwards
 * compatibility. `isEnabled` will now synchronously check if we're actively
 * profiling or not. This is necessary because we don't have callbacks to know
 * when profiling has started/stopped on Android APIs.
 */
export declare function setEnabled(_doEnable: boolean): void;
/**
 * Marks the start of a synchronous event that should end in the same stack
 * frame. The end of this event should be marked using the `endEvent` function.
 */
export declare function beginEvent(eventName: EventName, args?: EventArgs): void;
/**
 * Marks the end of a synchronous event started in the same stack frame.
 */
export declare function endEvent(args?: EventArgs): void;
/**
 * Traces the execution of the given function by marking its start with
 * `beginEvent` and its end with `endEvent`, even if the function throws.
 *
 * @example
 * Systrace.trace('myEvent', () => {
 *   // logic to trace
 * });
 */
export declare function trace<T>(eventName: EventName, fn: () => T, args?: EventArgs): T;
/**
 * Marks the start of a potentially asynchronous event. The end of this event
 * should be marked calling the `endAsyncEvent` function with the cookie
 * returned by this function.
 */
export declare function beginAsyncEvent(eventName: EventName, args?: EventArgs): number;
/**
 * Marks the end of a potentially asynchronous event, which was started with
 * the given cookie.
 */
export declare function endAsyncEvent(eventName: EventName, cookie: number, args?: EventArgs): void;
/**
 * Registers a new value for a counter event.
 */
export declare function counterEvent(eventName: EventName, value: number): void;
