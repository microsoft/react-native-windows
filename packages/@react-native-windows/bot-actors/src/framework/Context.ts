/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Webhooks} from '@octokit/webhooks';
import {Octokit} from '@octokit/rest';

import {ActorEventSource, Logger} from 'bot-actors';

/**
 * The actor context allows an actor to subscribe to events and perform actions
 * in the context of the function application
 */
export type Context = {
  log: Logger;
  octokit: Octokit;
  webhooks: Webhooks;
  events: ActorEventSource;
};
