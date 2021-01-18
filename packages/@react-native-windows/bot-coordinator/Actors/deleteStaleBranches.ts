/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {EventPayloads, WebhookEvent} from '@octokit/webhooks';
import {ActorRegistry} from './Actor';

/**
 * Actor to delete bot-created pull requests once they are closed or merged
 */
export default ActorRegistry.register('deleteStaleBranches', async context => {
  context.webhooks.on('pull_request.closed', onPullClosedOrMerged);
  context.webhooks.on('pull_request.merged', onPullClosedOrMerged);

  async function onPullClosedOrMerged(
    event: WebhookEvent<EventPayloads.WebhookPayloadPullRequest>,
  ) {
    const pull = event.payload.pull_request;
    const shouldDeleteBranch =
      pull.user.login === 'rnbot' &&
      (pull.head.user.login === 'microsoft' ||
        pull.head.user.login === 'rnbot');

    if (shouldDeleteBranch) {
      context.log(`Deleting stale branch "${pull.head.label}"`);
      await context.octokit.git.deleteRef({
        owner: pull.head.repo.owner.login,
        repo: pull.head.repo.name,
        ref: pull.head.ref,
      });
    } else {
      context.log.verbose(
        `Branch "${pull.head.label}" is not a candidate for deletion`,
      );
    }
  }
});
