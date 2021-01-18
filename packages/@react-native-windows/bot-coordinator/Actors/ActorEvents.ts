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
type ActorEvents = {
  'integration-timer-fired': () => Promise<void>;
};

export type ActorEventOn = <T extends keyof ActorEvents>(
  eventName: T,
  listener: ActorEvents[T],
) => ActorEventSink;

export type ActorEventEmit = <T extends keyof ActorEvents>(
  eventName: T,
  ...args: Parameters<ActorEvents[T]>
) => Promise<boolean>;

export type ActorEventSink = {
  on: ActorEventOn;
};

export type ActorEventSource = {
  emit: ActorEventEmit;
};

/**
 * Minimal Node EventEmitter-like interface which allows emitting events to
 * registered listeners. Emitting returns a promise that is fulfilled when
 * every handler has completed.
 */
export class ActorEventEmitter {
  private readonly listeners: {
    [E in keyof ActorEvents]?: Array<ActorEvents[E]>;
  } = {};

  on: ActorEventOn = (eventName, listener) => {
    if (!this.listeners[eventName]) {
      this.listeners[eventName] = [];
    }

    this.listeners[eventName]!.push(listener);
    return this;
  };

  emit: ActorEventEmit = async (eventName: keyof ActorEvents, ...args) => {
    const listeners = this.listeners[eventName];
    if (!listeners || listeners.length === 0) {
      return false;
    }

    for (const listener of listeners) {
      // @ts-ignore TypeScript can't figure out the correlation between mapped
      // value and params
      await listener(...args);
    }

    return true;
  };
}
