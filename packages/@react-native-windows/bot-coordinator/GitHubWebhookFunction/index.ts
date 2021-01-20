/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {WebhookEvent} from '@octokit/webhooks';
import {httpBotFunction} from '../botFunction';

export default httpBotFunction(async ({context, req, actorsHandle}) => {
  if (!req.headers['X-GitHub-Delivery'] || !req.headers['X-GitHub-Event']) {
    context.res = {status: 400, body: 'Missing headers'};
    return;
  }

  await actorsHandle.receiveWebhook({
    id: req.headers['X-GitHub-Delivery'],
    name: req.headers['X-GitHub-Event'] as WebhookEvent<any>['name'],
    payload: req.body,
  });
});
