/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<ae30ac1583432b8d710592f34758e827>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\vendor\emitter\EventEmitter.js
 */

type UnsafeEventObject = Object;
export interface EventSubscription {
  remove(): void;
}
export interface IEventEmitter<TEventToArgsMap extends Readonly<Record<string, ReadonlyArray<UnsafeEventObject>>>> {
  addListener<TEvent extends keyof TEventToArgsMap>(eventType: TEvent, listener: (...args: TEventToArgsMap[TEvent]) => unknown, context?: unknown): EventSubscription;
  emit<TEvent extends keyof TEventToArgsMap>(eventType: TEvent, ...args: TEventToArgsMap[TEvent]): void;
  removeAllListeners<TEvent extends keyof TEventToArgsMap>(eventType?: TEvent | undefined): void;
  listenerCount<TEvent extends keyof TEventToArgsMap>(eventType: TEvent): number;
}
/**
 * EventEmitter manages listeners and publishes events to them.
 *
 * EventEmitter accepts a single type parameter that defines the valid events
 * and associated listener argument(s).
 *
 * @example
 *
 *   const emitter = new EventEmitter<{
 *     success: [number, string],
 *     error: [Error],
 *   }>();
 *
 *   emitter.on('success', (statusCode, responseText) => {...});
 *   emitter.emit('success', 200, '...');
 *
 *   emitter.on('error', error => {...});
 *   emitter.emit('error', new Error('Resource not found'));
 *
 */
declare class EventEmitter<TEventToArgsMap extends Readonly<Record<string, ReadonlyArray<UnsafeEventObject>>> = Readonly<Record<string, ReadonlyArray<UnsafeEventObject>>>> implements IEventEmitter<TEventToArgsMap> {
  constructor();
  /**
   * Registers a listener that is called when the supplied event is emitted.
   * Returns a subscription that has a `remove` method to undo registration.
   */
  addListener<TEvent extends keyof TEventToArgsMap>(eventType: TEvent, listener: (...args: TEventToArgsMap[TEvent]) => unknown, context: unknown): EventSubscription;
  /**
   * Emits the supplied event. Additional arguments supplied to `emit` will be
   * passed through to each of the registered listeners.
   *
   * If a listener modifies the listeners registered for the same event, those
   * changes will not be reflected in the current invocation of `emit`.
   */
  emit<TEvent extends keyof TEventToArgsMap>(eventType: TEvent, ...args: TEventToArgsMap[TEvent]): void;
  /**
   * Removes all registered listeners.
   */
  removeAllListeners<TEvent extends keyof TEventToArgsMap>(eventType?: null | undefined | TEvent): void;
  /**
   * Returns the number of registered listeners for the supplied event.
   */
  listenerCount<TEvent extends keyof TEventToArgsMap>(eventType: TEvent): number;
}
export default EventEmitter;
