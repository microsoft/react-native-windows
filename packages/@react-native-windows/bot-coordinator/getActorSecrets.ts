/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Secrets} from '@react-native-windows/bot-actors';

export default function getActorSecrets(): Secrets {
  const githubAuthToken = process.env.GITHUB_AUTH_TOKEN;
  if (githubAuthToken === undefined) {
    throw new Error('"GITHUB_AUTH_TOKEN" env variable must be set');
  }

  const githubWebhookSecret = process.env.GITHUB_WEBHOOK_SECRET;
  if (githubWebhookSecret === undefined) {
    throw new Error('"GITHUB_WEBHOOK_SECRET" env variable must be set');
  }

  return {githubAuthToken, githubWebhookSecret};
}
