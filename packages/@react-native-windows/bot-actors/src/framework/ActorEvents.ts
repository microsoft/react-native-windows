/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Events that map be fired from functions to actors, including the expected
 * signature of listeners
 */
export type ActorEvents = {
  'integration-timer-fired': () => Promise<void>;
};

/**
 * Minimal Node EventEmitter-like interface which allows emitting events to
 * registered listeners. Emitting returns a promise that is fulfilled when
 * every handler has completed.
 */
export type ActorEventEmitter = {
  on: OnActorEvent;
  emit: EmitActorEvent;
};

export type OnActorEvent = <T extends keyof ActorEvents>(
  eventName: T,
  listener: ActorEvents[T],
) => ActorEventSource;

export type EmitActorEvent = <T extends keyof ActorEvents>(
  eventName: T,
  ...args: Parameters<ActorEvents[T]>
) => Promise<boolean>;

export type ActorEventSource = {
  on: OnActorEvent;
};

export type ActorEventSink = {
  emit: EmitActorEvent;
};
