/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Webhooks, WebhookEvent} from '@octokit/webhooks';
import {Octokit} from '@octokit/rest';
import {retry} from '@octokit/plugin-retry';
import {throttling} from '@octokit/plugin-throttling';

import {
  Context,
  ActorEventEmitter,
  ActorEvents,
  ActorDefinition,
  Logger,
  Secrets,
  EmitActorEvent,
  OnActorEvent,
} from 'bot-actors';

export class ActorInstance {
  private constructor(
    private readonly actorName: string,
    private readonly logger: Logger,
    private readonly octokit: Octokit,
    private readonly webhooks: Webhooks,
  ) {}

  eventEmitter: ActorEventEmitter = new ActorEventEmitterImpl();

  public static async initialize(
    actorName: string,
    actor: ActorDefinition,
    logger: Logger,
    secrets: Secrets,
  ) {
    const decoratedLogger = decorateWithPrefix(logger, actorName);
    const octokit = createOctokit(decoratedLogger, secrets);
    const webhooks = new Webhooks({
      secret: secrets.githubWebhookSecret,
    });

    const instance = new ActorInstance(
      actorName,
      decoratedLogger,
      octokit,
      webhooks,
    );

    await actor(instance.context());
    return instance;
  }

  context(): Context {
    return {
      log: this.logger,
      octokit: this.octokit,
      webhooks: this.webhooks,
      events: this.eventEmitter,
    };
  }

  emitEvent: EmitActorEvent = async (eventName, ...args) => {
    this.logger.info(`Receiving event "${eventName}"`);

    if (await this.eventEmitter.emit(eventName, ...args)) {
      this.logger.info(
        `Actor "${this.actorName}" accepted the event "${eventName}"`,
      );
      return true;
    } else {
      this.logger.verbose(
        `Actor "${this.actorName}" rejected the event "${eventName}"`,
      );
      return false;
    }
  };

  receiveWebhook: (
    event: WebhookEvent & {signature: string},
  ) => Promise<void> = async event => {
    this.logger.info(`Receiving webhook "${webhookToString(event)}"`);

    this.logger.verbose(
      `Offering actor "${this.actorName}" webhook "${webhookToString(event)}"`,
    );
    await this.webhooks.verifyAndReceive(event);
  };
}

function decorateWithPrefix(log: Logger, prefix: string): Logger {
  return {
    error: (...args: any[]) => log.error(`[${prefix}]`, ...args),
    warn: (...args: any[]) => log.warn(`[${prefix}]`, ...args),
    info: (...args: any[]) => log.info(`[${prefix}]`, ...args),
    verbose: (...args: any[]) => log.verbose(`[${prefix}]`, ...args),
  };
}

type OctokitThrottleOptions = {
  request: {retryCount: number};
  method: string;
  url: string;
};

function createOctokit(logger: Logger, secrets: Secrets): Octokit {
  const ActorOctokit = Octokit.plugin(retry, throttling);

  return new ActorOctokit({
    auth: secrets.githubAuthToken,
    log: {
      ...logger,
      debug: logger.verbose,
    },
    throttle: {
      onRateLimit: (retryAfter: number, options: OctokitThrottleOptions) => {
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
      onAbuseLimit: (_retryAfter: number, options: OctokitThrottleOptions) => {
        logger.error(
          `Abuse limit hit for request ${options.method} ${options.url}`,
        );
        return false;
      },
    },
  });
}

class ActorEventEmitterImpl implements ActorEventEmitter {
  private readonly listeners: {
    [E in keyof ActorEvents]?: Array<ActorEvents[E]>;
  } = {};

  on: OnActorEvent = (eventName, listener) => {
    if (!this.listeners[eventName]) {
      this.listeners[eventName] = [];
    }

    this.listeners[eventName]!.push(listener);
    return this;
  };

  emit: EmitActorEvent = async (eventName: keyof ActorEvents, ...args) => {
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

function webhookToString(event: WebhookEvent): string {
  if (event.payload.action) {
    return `${event.name}.${event.payload.action}`;
  } else {
    return event.name;
  }
}
