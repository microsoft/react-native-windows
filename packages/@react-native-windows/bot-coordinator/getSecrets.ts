/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Secrets as ActorSecrets} from '@react-native-windows/bot-actors';

export type Secrets = ActorSecrets & {
  /** HMAC SHA-256 Secret used to validate messages */
  httpSharedSecret: string;
};

export default function getSecrets(): Secrets {
  const githubAuthToken = process.env.GITHUB_AUTH_TOKEN;
  if (githubAuthToken === undefined) {
    throw new Error('"GITHUB_AUTH_TOKEN" env variable must be set');
  }

  const httpSharedSecret = process.env.HTTP_SHARED_SECRET;
  if (httpSharedSecret === undefined) {
    throw new Error('"HTTP_SHARED_SECRET" env variable must be set');
  }

  return {githubAuthToken, httpSharedSecret};
}
