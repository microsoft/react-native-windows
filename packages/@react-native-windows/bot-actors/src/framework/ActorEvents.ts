/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {AsyncEventEmitter} from 'bot-actors';

/**
 * Events that map be fired from functions to actors, including the expected
 * signature of listeners
 */
export type ActorEvents = {
  'integration-timer-fired': () => Promise<void>;
};

export type ActorEventEmitter = AsyncEventEmitter<ActorEvents>;
