/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Context, HttpRequest} from '@azure/functions';
import {WebhookEvent} from '@octokit/webhooks';
import {initializeActors} from '@react-native-windows/bot-actors';
import getActorSecrets from '../getActorSecrets';

export default async (context: Context, req: HttpRequest) => {
  const actorsHandle = await initializeActors(context.log, getActorSecrets());

  try {
    await actorsHandle.receiveWebhook({
      id: req.headers['x-github-delivery'],
      name: req.headers['x-github-event'] as WebhookEvent<any>['name'],
      payload: req.body,
      signature: req.headers['x-hub-signature'],
    });
  } catch (ex) {
    context.res = {status: 500};
    throw ex;
  }

  context.res = {status: 200};
};
