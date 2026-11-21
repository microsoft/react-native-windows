/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<93c4ca4cf575bd12c53e3d1ef1b43dd9>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\events\EventTarget.js
 */

import Event from "./Event";
export type EventCallback = (event: Event) => void;
export type EventHandler = {
  handleEvent(event: Event): void;
};
export type EventListener = EventCallback | EventHandler;
export type EventListenerOptions = Readonly<{
  capture?: boolean | undefined;
}>;
export type AddEventListenerOptions = Readonly<Omit<EventListenerOptions, "passive" | "once" | "signal"> & {
  passive?: boolean | undefined;
  once?: boolean | undefined;
  signal?: AbortSignal | undefined;
}>;
declare class EventTarget {
  addEventListener(type: string, callback: EventListener | null, optionsOrUseCapture?: AddEventListenerOptions | boolean): void;
  removeEventListener(type: string, callback: EventListener, optionsOrUseCapture?: EventListenerOptions | boolean): void;
  dispatchEvent(event: Event): boolean;
}
export default EventTarget;
