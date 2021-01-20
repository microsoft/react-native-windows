/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as crypto from 'crypto';

import {
  ActorsHandle,
  initializeActors,
  Secrets as ActorSecrets,
} from '@react-native-windows/bot-actors';
import {AzureFunction, Context, HttpRequest} from '@azure/functions';

export type Secrets = ActorSecrets & {
  /** HMAC SHA-256 Secret used to validate messages */
  httpSharedSecret: string;
};

export type ActorFunctionDelegate = (opts: {
  context: Context;
  secrets: Secrets;
  actorsHandle: ActorsHandle;
}) => Promise<void>;

export function actorFunction(delegate: ActorFunctionDelegate): AzureFunction {
  return async (context: Context) => {
    const secrets = getSecrets();

    const actorsHandle = await initializeActors(context.log, secrets);
    await delegate({context, secrets, actorsHandle});
  };
}

export type HttpFunctionDelegate = (opts: {
  context: Context;
  secrets: Secrets;
  actorsHandle: ActorsHandle;
  req: HttpRequest;
}) => Promise<void>;
export function httpFunction(delegate: HttpFunctionDelegate) {
  return async (context: Context, req: HttpRequest) => {
    const secrets = getSecrets();

    if (!verifyMessageSignature(context, req, secrets)) {
      context.log.error('Signature verification failed');
      context.res = {status: 403, body: 'Signature verification failed'};
      return;
    }

    const actorsHandle = await initializeActors(context.log, secrets);
    await delegate({context, secrets, actorsHandle, req});
  };
}

/**
 * Verifies whether an HTTP request has been correctly signed with the
 * pre-shared key.
 */
function verifyMessageSignature(
  context: Context,
  req: HttpRequest,
  secrets: Secrets,
) {
  if (!req.rawBody) {
    context.log.error('No body present in HTTP request');
    return false;
  }

  const givenSignature =
    req.headers['X-Hub-Signature-256'] || req.headers['X-Bot-Signature-256'];
  if (!givenSignature) {
    context.log.error('No signature header present in HTTP request');
    return false;
  }

  const hmac = crypto.createHmac('sha256', secrets.httpSharedSecret);
  hmac.update(req.rawBody);
  const expectedSignature = hmac.digest('hex');

  if (givenSignature.toLowerCase() !== expectedSignature.toLowerCase()) {
    context.log.error('Payload signature did not match expectations');
    return false;
  }

  return true;
}

function getSecrets(): Secrets {
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
