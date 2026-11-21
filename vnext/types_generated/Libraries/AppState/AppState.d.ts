/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<af297ce6a279ac26347b43994616ef77>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\AppState\AppState.js
 */

import { type EventSubscription } from "../vendor/emitter/EventEmitter";
/**
 * The app's current state.
 *
 * - `active` — The app is running in the foreground.
 * - `background` — The app is running in the background. The user is either
 *   in another app, on the home screen, or (Android only) on another Activity,
 *   including temporary system activities such as autofill credential pickers.
 * - `inactive` — A transitional state that occurs when moving between
 *   foreground and background, and during periods of inactivity such as
 *   entering the multitasking view, opening the Notification Center, or in the
 *   event of an incoming call.
 *
 * @platform ios `inactive`
 */
export type AppStateStatus = "inactive" | "background" | "active" | "extension" | "unknown";
/**
 * Events emitted by `AppState`.
 *
 * - `change` — Received when the app state has changed.
 * - `memoryWarning` — Received when the system issues a memory warning.
 * - `focus` — Received when the app gains focus (the user is interacting
 *   with the app).
 * - `blur` — Received when the user is not actively interacting with the app.
 *
 * @platform android `focus`, `blur`
 */
/**
 * Events emitted by `AppState`.
 *
 * - `change` — Received when the app state has changed.
 * - `memoryWarning` — Received when the system issues a memory warning.
 * - `focus` — Received when the app gains focus (the user is interacting
 *   with the app).
 * - `blur` — Received when the user is not actively interacting with the app.
 *
 * @platform android `focus`, `blur`
 */
type AppStateEventDefinitions = {
  change: [AppStateStatus];
  memoryWarning: [];
  blur: [];
  focus: [];
};
export type AppStateEvent = keyof AppStateEventDefinitions;
/**
 * Reports the app's current state (`active`, `background`, or `inactive`) and
 * notifies when it changes. Frequently used to handle push notification
 * behavior.
 *
 * @see https://reactnative.dev/docs/appstate
 */
declare class AppStateImpl {
  /**
   * The current app state. Can be `null` until the initial value is set.
   */
  currentState: null | undefined | string;
  isAvailable: boolean;
  constructor();
  /**
   * Add a handler to `AppState` changes by listening to the `change` event
   * type and providing the handler. See `AppStateEvent` for the list of
   * available events.
   */
  addEventListener<K extends AppStateEvent>(type: K, handler: (...$$REST$$: AppStateEventDefinitions[K]) => void): EventSubscription;
}
declare const AppState: AppStateImpl;
declare const $$AppState: typeof AppState;
declare type $$AppState = typeof $$AppState;
export default $$AppState;
