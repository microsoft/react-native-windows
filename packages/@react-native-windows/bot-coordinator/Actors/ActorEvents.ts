/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

type ActorEvents = {
  'integration-timer-fired': () => void;
};

export type ActorEventOn = <T extends keyof ActorEvents>(
  eventName: T,
  listener: ActorEvents[T],
) => ActorEventSink;
export type ActorEventEmit = <T extends keyof ActorEvents>(
  eventName: T,
  ...args: Parameters<ActorEvents[T]>
) => boolean;

export type ActorEventSink = {
  on: ActorEventOn;
};

export type ActorEventSource = {
  emit: ActorEventEmit;
};

export type ActorEventEmitter = {
  on: ActorEventOn;
  emit: ActorEventEmit;
};
