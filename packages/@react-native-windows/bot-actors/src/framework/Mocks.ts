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
} from 'bot-actors';

export const failOnErrorLogger: Logger = {
  verbose: () => {},
  info: () => {},
  warn: () => {},
  error: (...args: any) => {
    fail(args.map((arg: any) => arg.toString()).join(' '));
  },
};

export const fakeSecrets: Secrets = {
  githubAuthToken: 'ABCD',
};

export class MockContext implements Context {
  log: Logger;
  octokit: Octokit;
  webhooks: Webhooks;
  events: ActorEventEmitter;

  constructor() {
    this.log = failOnErrorLogger;
    this.octokit = new Octokit();
    this.webhooks = new Webhooks();
    this.events = new AsyncEventEmitter();
  }

  async receiveWebhook(event: WebhookEvent) {
    await this.webhooks.receive(event);
  }

  emitEvent: ActorEventEmitter['emit'] = async (eventName, ...args) => {
    return await this.events.emit(eventName, ...args);
  };
}
