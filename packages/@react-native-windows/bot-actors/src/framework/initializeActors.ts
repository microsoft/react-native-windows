/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as globby from 'globby';

import {WebhookEvent} from '@octokit/webhooks';
import {ActorRegistry, Secrets, EmitActorEvent} from 'bot-actors';
import {ActorInstance} from './ActorInstance';

/**
 * ActorsHandle allows external interaction with the collection of initialized
 * actors, such as triggering events
 */
export type ActorsHandle = {
  emitEvent: EmitActorEvent;
  receiveWebhook: (event: WebhookEvent & {signature: string}) => Promise<void>;
};

/**
 * Log output for the actors system
 */
export type Logger = {
  verbose: (...args: any) => void;
  info: (...args: any) => void;
  warn: (...args: any) => void;
  error: (...args: any) => void;
};

/**
 * Attempts to perform global registration of all actors present, then runs
 * them to create event subscriptions. Returns the ActorsHandle to allow
 * functions to trigger an event to actors.
 */
export async function initializeActors(
  logger: Logger,
  secrets: Secrets,
): Promise<ActorsHandle> {
  if (globalActorInstances.length > 0) {
    throw new Error('Actors have already been initialized');
  }

  const actorSources = await globby('**', {
    cwd: `${__dirname}/..`,
    absolute: true,
  });
  for (const sourceFile of actorSources) {
    logger.verbose(`Loading "${sourceFile}"`);
    require(sourceFile);
  }

  for (const [actorName, actor] of Object.entries(ActorRegistry.getActors())) {
    globalActorInstances.push(
      await ActorInstance.initialize(actorName, actor, logger, secrets),
    );
  }

  return {
    emitEvent: async (eventName, ...args) => {
      let eventReceived = false;
      for (const instance of globalActorInstances) {
        eventReceived =
          eventReceived || (await instance.emitEvent(eventName, ...args));
      }

      return eventReceived;
    },

    receiveWebhook: async event => {
      for (const instance of globalActorInstances) {
        await instance.receiveWebhook(event);
      }
    },
  };
}

const globalActorInstances: ActorInstance[] = [];
