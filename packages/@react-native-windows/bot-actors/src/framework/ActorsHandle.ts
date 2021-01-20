/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {WebhookEvent} from '@octokit/webhooks';
import {ActorInstance, ActorEventEmitter} from 'bot-actors';

/**
 * GitHub webhook event with packaged payload signature
 */
export type SignedWebhookEvent = WebhookEvent & {signature: string};

/**
 * ActorsHandle allows external interaction with the collection of initialized
 * actors, such as triggering events
 */
export type ActorsHandle = {
  emitEvent: ActorEventEmitter['emit'];
  receiveWebhook: (event: SignedWebhookEvent) => Promise<void>;
};

export class ActorsHandleImpl implements ActorsHandle {
  constructor(private readonly instances: ActorInstance[]) {}

  emitEvent: ActorEventEmitter['emit'] = async (eventName, ...args) => {
    let eventReceived = false;
    for (const instance of this.instances) {
      eventReceived =
        eventReceived || (await instance.emitEvent(eventName, ...args));
    }

    return eventReceived;
  };

  receiveWebhook = async (event: SignedWebhookEvent) => {
    for (const instance of this.instances) {
      await instance.receiveWebhook(event);
    }
  };
}
