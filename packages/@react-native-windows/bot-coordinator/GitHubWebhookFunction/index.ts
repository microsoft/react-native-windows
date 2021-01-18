/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Context, HttpRequest} from '@azure/functions';
import {initializeActors} from '../Actors/Actor';
import {WebhookEvent} from '@octokit/webhooks';

export default async (context: Context, req: HttpRequest) => {
  const actorsHandle = await initializeActors(context);

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
