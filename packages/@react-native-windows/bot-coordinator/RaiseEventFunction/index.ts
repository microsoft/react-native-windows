/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {WebhookEvent} from '@octokit/webhooks';
import {httpFunction} from '../actorFunction';

export default httpFunction(async ({context, req, actorsHandle}) => {
  try {
    await actorsHandle.receiveWebhook({
      id: req.headers['X-GitHub-Delivery'],
      name: req.headers['X-GitHub-Event'] as WebhookEvent<any>['name'],
      payload: req.body,
    });
  } catch (ex) {
    context.res = {status: 500};
    throw ex;
  }

  context.res = {status: 200};
});
