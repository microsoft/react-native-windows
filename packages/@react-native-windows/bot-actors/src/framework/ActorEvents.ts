/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as t from 'io-ts';
import {AsyncEventEmitter} from './AsyncEventEmitter';

/**
 * Events that map be fired from functions to actors, including the expected
 * event payload
 */
export const actorEvents = {
  'integration-timer-fired': t.undefined,
  'publish-pipeline-failed': t.type({
    buildNumber: t.string,
    buildUri: t.string,
  }),
};

export type ActorEventName = keyof typeof actorEvents;

type EventType<T extends ActorEventName> = t.TypeOf<typeof actorEvents[T]>;

export type ActorEventCallbacks = {
  [E in keyof typeof actorEvents]: EventType<E> extends undefined
    ? () => Promise<void>
    : (args: EventType<E>) => Promise<void>;
};

export type ActorEventEmitter = AsyncEventEmitter<ActorEventCallbacks>;
