/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Octokit} from '@octokit/rest';
import {WebhookEvent, Webhooks} from '@octokit/webhooks';
import {
  ActorEventEmitter,
  AsyncEventEmitter,
  Context,
  Logger,
  Secrets,
} from '../framework';

export const failOnErrorLogger: Logger = {
  verbose: console.log,
  info: console.log,
  warn: console.warn,
  error: (...args: any) => {
    fail(args.map((arg: any) => arg.toString()).join(' '));
  },
};

export const fakeSecrets: Secrets = {
  githubAuthToken: 'ABCD',
  githubWebhookSecret: 'ABCD',
};

export class MockContext implements Context {
  log: Logger;
  octokit: Octokit;
  webhooks: Webhooks;
  events: ActorEventEmitter;

  constructor() {
    this.log = failOnErrorLogger;
    this.octokit = new Octokit();
    this.webhooks = new Webhooks({secret: 'ABCD'});
    this.events = new AsyncEventEmitter();
  }

  async receiveWebhook(event: WebhookEvent) {
    await this.webhooks.receive(event);
  }

  emitEvent: ActorEventEmitter['emit'] = async (eventName, ...args) => {
    return await this.events.emit(eventName, ...args);
  };
}
