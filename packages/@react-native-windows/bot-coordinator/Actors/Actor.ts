/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as globby from 'globby';

import {Context as FunctionContext} from '@azure/functions';
import {Octokit} from '@octokit/rest';
import {Webhooks, WebhookEvent} from '@octokit/webhooks';

import {ActorEventSink, ActorEventEmit, ActorEventEmitter} from './ActorEvents';

/**
 * Actor functions allow creating flows that react to external events, such as
 * GitHub events or function triggers
 */
export type Actor = (context: ActorContext) => Promise<void>;

/**
 * The actor context allows an actor to subscribe to events and perform actions
 * in the context of the function application
 */
export type ActorContext = FunctionContext & {
  octokit: Octokit;
  webhooks: Webhooks;
  actorEvents: ActorEventSink;
};

/**
 * Internal state stored per-initialized actor
 */
interface ActorInstance extends ActorContext {
  actorName: string;
  actorEvents: ActorEventEmitter;
}

/**
 * ActorsHandle allows external interaction with the collection of initialized
 * actors, such as triggering ecents
 */
export type ActorsHandle = {
  emit: ActorEventEmit;
  receiveWebhook: (event: WebhookEvent & {signature: string}) => Promise<void>;
};

const globallyRegisteredActors: Record<string, Actor> = {};
const globalActorInstances: ActorInstance[] = [];

/**
 * Public access point to globally register actors
 */
export const ActorRegistry = {
  register: (actorName: string, actor: Actor) => {
    if (globallyRegisteredActors.hasOwnProperty(actorName)) {
      throw new Error(`Actor "${actorName}" has already been registered`);
    }

    globallyRegisteredActors[actorName] = actor;
  },
};

/**
 * Attempts to perform global registration of all actors present, then runs
 * them to create event subscriptions. Returns the ActorsHandle to allow
 * functions to trigger an event to actors.
 */
export async function initializeActors(
  funcContext: FunctionContext,
): Promise<ActorsHandle> {
  const actorSources = await globby('**', {cwd: __dirname, absolute: true});
  for (const sourceFile of actorSources) {
    funcContext.log.verbose(`Loading "${sourceFile}"`);
    require(sourceFile);
  }

  const octokit = createOctokit(funcContext);
  for (const [actorName, actor] of Object.entries(globallyRegisteredActors)) {
    globalActorInstances.push(
      await createActorInstance(actorName, actor, {funcContext, octokit}),
    );
  }

  return createActorsHandle(funcContext, globalActorInstances);
}

async function createActorInstance(
  actorName: string,
  actor: Actor,
  resources: {
    funcContext: FunctionContext;
    octokit: Octokit;
  },
): Promise<ActorInstance> {}

function createActorsHandle(
  funcContext: FunctionContext,
  actorInstances: ActorInstance[],
): ActorsHandle {
  return {
    emit: (eventName, ...args) => {
      funcContext.log(`Receiving event "${eventName}"`);

      let anyActorAcceptedEvent = false;
      for (const instance of actorInstances) {
        if (instance.actorEvents.emit(eventName, ...args)) {
          funcContext.log(
            `Actor "${instance.actorName}" accepted the event "${eventName}"`,
          );
          anyActorAcceptedEvent = true;
        } else {
          funcContext.log.verbose(
            `Actor "${instance.actorName}" rejected the event "${eventName}"`,
          );
        }
      }

      return anyActorAcceptedEvent;
    },

    receiveWebhook: async event => {
      funcContext.log(`Receiving webhook "${webhookToString(event)}"`);
      for (const instance of actorInstances) {
        funcContext.log.verbose(
          `Offering actor "${instance.actorName}" webhook "${webhookToString(
            event,
          )}"`,
        );
        await instance.webhooks.verifyAndReceive(event);
      }
    },
  };
}

function webhookToString(event: WebhookEvent) {
  if (event.payload.action) {
    return `${event.name}.${event.payload.action}`;
  } else {
    return event.name;
  }
}

function createOctokit(funcContext: FunctionContext): Octokit {}

function createWebhooks(funcContext: FunctionContext): Webhooks {
  return new Webhooks({
    secret: process.env.GITHUB_WEBHOOK_SECRET,
  });
}
