/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<6d158c724798dfc81e24f4b01e5f9916>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\events\Event.js
 */

/**
 * This module implements the `Event` interface from the DOM.
 * See https://dom.spec.whatwg.org/#interface-event.
 */

import type EventTarget from "./EventTarget";
export interface EventInit {
  readonly bubbles?: boolean | undefined;
  readonly cancelable?: boolean | undefined;
  readonly composed?: boolean | undefined;
  readonly rnIsDirect?: boolean | undefined;
}
declare class Event {
  static readonly NONE: 0;
  static readonly CAPTURING_PHASE: 1;
  static readonly AT_TARGET: 2;
  static readonly BUBBLING_PHASE: 3;
  readonly NONE: 0;
  readonly CAPTURING_PHASE: 1;
  readonly AT_TARGET: 2;
  readonly BUBBLING_PHASE: 3;
  constructor(type: string, options?: null | undefined | EventInit);
  get bubbles(): boolean;
  get cancelable(): boolean;
  /**
   * Historical alias for the stop-propagation flag. Reading it returns whether
   * `stopPropagation()` has been called (or `cancelBubble` has been set to
   * `true`). Setting it to `true` stops propagation; setting it to `false` is a
   * no-op. See https://dom.spec.whatwg.org/#dom-event-cancelbubble.
   */
  get cancelBubble(): boolean;
  set cancelBubble(value: boolean);
  get composed(): boolean;
  /**
   * React Native-specific. When true, this event is dispatched only to its
   * target (single `AT_TARGET` phase) and never captures or bubbles through
   * ancestors.
   */
  get rnIsDirect(): boolean;
  get currentTarget(): EventTarget | null;
  get defaultPrevented(): boolean;
  get eventPhase(): EventPhase;
  get isTrusted(): boolean;
  get target(): EventTarget | null;
  get timeStamp(): number;
  get type(): string;
  composedPath(): ReadonlyArray<EventTarget>;
  preventDefault(): void;
  stopImmediatePropagation(): void;
  stopPropagation(): void;
}
export default Event;
export type EventPhase = (typeof Event)["NONE"] | (typeof Event)["CAPTURING_PHASE"] | (typeof Event)["AT_TARGET"] | (typeof Event)["BUBBLING_PHASE"];
