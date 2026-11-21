/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<d118080935a1486858bda4192be05223>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\EventEmitter\NativeEventEmitter.js
 */

import type { EventSubscription, IEventEmitter } from "../vendor/emitter/EventEmitter";
/**
 * The React Native implementation of the IOS RCTEventEmitter which is required when creating
 * a module that communicates with IOS
 */
/**
 * The React Native implementation of the IOS RCTEventEmitter which is required when creating
 * a module that communicates with IOS
 */
interface NativeModule {
  /**
   * Add the provided eventType as an active listener
   * @param eventType name of the event for which we are registering listener
   */
  addListener(eventType: string): void;
  /**
   * Remove a specified number of events.  There are no eventTypes in this case, as
   * the native side doesn't remove the name, but only manages a counter of total
   * listeners
   * @param count number of listeners to remove (of any type)
   */
  removeListeners(count: number): void;
}
/** @deprecated Use `EventSubscription` instead. */
/** @deprecated Use `EventSubscription` instead. */
type EmitterSubscription = EventSubscription;
export type { EventSubscription, EmitterSubscription };
/** @deprecated Use `EventSubscription` instead. */
export type NativeEventSubscription = EventSubscription;
type UnsafeNativeEventObject = Object;
/**
 * `NativeEventEmitter` is intended for use by Native Modules to emit events to
 * JavaScript listeners. If a `NativeModule` is supplied to the constructor, it
 * will be notified (via `addListener` and `removeListeners`) when the listener
 * count changes to manage "native memory".
 *
 * Currently, all native events are fired via a global `RCTDeviceEventEmitter`.
 * This means event names must be globally unique, and it means that call sites
 * can theoretically listen to `RCTDeviceEventEmitter` (although discouraged).
 */
declare class NativeEventEmitter<TEventToArgsMap extends Readonly<Record<string, ReadonlyArray<UnsafeNativeEventObject>>> = Readonly<Record<string, ReadonlyArray<UnsafeNativeEventObject>>>> implements IEventEmitter<TEventToArgsMap> {
  /**
   * @param nativeModule the NativeModule implementation.  This is required on IOS and will throw
   *      an invariant error if undefined.
   */
  constructor(nativeModule?: null | undefined | NativeModule);
  /**
   * Add the specified listener, this call passes through to the NativeModule
   * addListener
   *
   * @param eventType name of the event for which we are registering listener
   * @param listener the listener function
   * @param context context of the listener
   */
  addListener<TEvent extends keyof TEventToArgsMap>(eventType: TEvent, listener: (...args: TEventToArgsMap[TEvent]) => unknown, context?: unknown): EventSubscription;
  emit<TEvent extends keyof TEventToArgsMap>(eventType: TEvent, ...args: TEventToArgsMap[TEvent]): void;
  /**
   * @param eventType  name of the event whose registered listeners to remove
   */
  removeAllListeners<TEvent extends keyof TEventToArgsMap>(eventType?: null | undefined | TEvent): void;
  listenerCount<TEvent extends keyof TEventToArgsMap>(eventType: TEvent): number;
}
export default NativeEventEmitter;
