/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as globby from 'globby';

import {Context as FunctionContext, Logger} from '@azure/functions';
import {Webhooks, WebhookEvent} from '@octokit/webhooks';
import {Octokit} from '@octokit/rest';
import {retry} from '@octokit/plugin-retry';
import {throttling} from '@octokit/plugin-throttling';

import {ActorEvents} from './ActorEvents';

/**
 * Actor functions allow creating flows that react to external events, such as
 * GitHub events or function triggers
 */
export type Actor = (context: ActorContext) => Promise<void>;

/**
 * The actor context allows an actor to subscribe to events and perform actions
 * in the context of the function application
 */
export type ActorContext = {
  log: Logger;
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

type ActorEventOn = <T extends keyof ActorEvents>(
  eventName: T,
  listener: ActorEvents[T],
) => ActorEventSink;

type ActorEventEmit = <T extends keyof ActorEvents>(
  eventName: T,
  ...args: Parameters<ActorEvents[T]>
) => Promise<boolean>;

type ActorEventSink = {
  on: ActorEventOn;
};

const globallyRegisteredActors: Record<string, Actor> = {};
const globalActorInstances: ActorInstance[] = [];

/**
 * Public access point to globally register actors
 */
export const ActorRegistry = {
  register: (actorName: string, actor: Actor): Actor => {
    if (globallyRegisteredActors.hasOwnProperty(actorName)) {
      throw new Error(`Actor "${actorName}" has already been registered`);
    }

    globallyRegisteredActors[actorName] = actor;
    return actor;
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

  for (const [actorName, actor] of Object.entries(globallyRegisteredActors)) {
    globalActorInstances.push(
      await createActorInstance(actorName, actor, funcContext),
    );
  }

  return createActorsHandle(funcContext, globalActorInstances);
}

/**
 * Minimal Node EventEmitter-like interface which allows emitting events to
 * registered listeners. Emitting returns a promise that is fulfilled when
 * every handler has completed.
 */
class ActorEventEmitter {
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

async function createActorInstance(
  actorName: string,
  actor: Actor,
  funcContext: FunctionContext,
): Promise<ActorInstance> {
  const log = decorateWithPrefix(funcContext.log, actorName);

  const instance: ActorInstance = {
    actorName,
    log: log,
    octokit: createOctokit(log),
    webhooks: new Webhooks({
      secret: process.env.GITHUB_WEBHOOK_SECRET,
    }),
    actorEvents: new ActorEventEmitter(),
  };

  await actor(instance);
  return instance;
}

function decorateWithPrefix(logger: Logger, prefix: string): Logger {
  return Object.assign((...args: any[]) => logger(`[${prefix}]`, ...args), {
    error: (...args: any[]) => logger.error(`[${prefix}]`, ...args),
    warn: (...args: any[]) => logger.warn(`[${prefix}]`, ...args),
    info: (...args: any[]) => logger.info(`[${prefix}]`, ...args),
    verbose: (...args: any[]) => logger.verbose(`[${prefix}]`, ...args),
  });
}

function createActorsHandle(
  funcContext: FunctionContext,
  actorInstances: ActorInstance[],
): ActorsHandle {
  return {
    emit: async (eventName, ...args) => {
      funcContext.log(`Receiving event "${eventName}"`);

      let anyActorAcceptedEvent = false;
      for (const instance of actorInstances) {
        if (await instance.actorEvents.emit(eventName, ...args)) {
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

function webhookToString(event: WebhookEvent): string {
  if (event.payload.action) {
    return `${event.name}.${event.payload.action}`;
  } else {
    return event.name;
  }
}

type ThrottleOptions = {
  request: {retryCount: number};
  method: string;
  url: string;
};

function createOctokit(logger: Logger): Octokit {
  const ActorOctokit = Octokit.plugin(retry, throttling);

  return new ActorOctokit({
    auth: process.env.GITHUB_AUTH_TOKEN,
    log: {
      debug: logger.verbose,
      info: logger.info,
      warn: logger.warn,
      error: logger.error,
    },
    throttle: {
      onRateLimit: (retryAfter: number, options: ThrottleOptions) => {
        logger.warn(
          `Request quota exhausted for request ${options.method} ${options.url}`,
        );

        if (options.request.retryCount < 3) {
          logger.info(`Retrying after ${retryAfter} seconds!`);
          return true;
        } else {
          logger.error(`Maximum retries exceeded`);
          return false;
        }
      },
      onAbuseLimit: (_retryAfter: number, options: ThrottleOptions) => {
        logger.error(
          `Abuse limit hit for request ${options.method} ${options.url}`,
        );
        return false;
      },
    },
  });
}
