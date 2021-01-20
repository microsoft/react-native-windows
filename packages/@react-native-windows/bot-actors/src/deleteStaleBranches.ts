/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {EventPayloads, WebhookEvent} from '@octokit/webhooks';

import {ActorRegistry} from 'bot-actors';

/**
 * Actor to delete bot-created branches once an associated pull request is closed or merged
 */
export default ActorRegistry.register(
  'deleteStaleBranches',
  async ({log, webhooks, octokit}) => {
    webhooks.on('pull_request.closed', onPullClosedOrMerged);
    webhooks.on('pull_request.merged', onPullClosedOrMerged);

    async function onPullClosedOrMerged(
      event: WebhookEvent<EventPayloads.WebhookPayloadPullRequest>,
    ) {
      const pull = event.payload.pull_request;
      const shouldDeleteBranch =
        pull.user.login === 'rnbot' &&
        (pull.head.user.login === 'microsoft' ||
          pull.head.user.login === 'rnbot');

      if (shouldDeleteBranch) {
        log.info(`Deleting stale branch "${pull.head.label}"`);
        await octokit.git.deleteRef({
          owner: pull.head.repo.owner.login,
          repo: pull.head.repo.name,
          ref: pull.head.ref,
        });
      } else {
        log.verbose(
          `Branch "${pull.head.label}" is not a candidate for deletion`,
        );
      }
    }
  },
);
