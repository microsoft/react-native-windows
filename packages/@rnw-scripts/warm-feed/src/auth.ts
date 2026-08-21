/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {spawnSync} from 'node:child_process';
import type {Auth, Logger} from './types';

// Azure DevOps resource id for AAD access tokens.
const ADO_RESOURCE = '499b84ac-1321-427f-aa17-267ca6975798';

function envToken(): string | undefined {
  return process.env.ADO_PAT || process.env.AZURE_DEVOPS_EXT_PAT || undefined;
}

function azToken(log: Logger): string | undefined {
  // All arguments are constant, so a single shell command string is safe and
  // avoids the args-with-shell deprecation (DEP0190). `az` is a .cmd on Windows.
  const res = spawnSync(
    `az account get-access-token --resource ${ADO_RESOURCE} --query accessToken -o tsv`,
    {encoding: 'utf8', shell: true},
  );
  if (res.status === 0 && res.stdout) return res.stdout.trim();
  log.debug(`az token acquisition failed: ${res.stderr.trim() || res.error}`);
  return undefined;
}

/**
 * Resolve feed auth. Order: a pre-acquired AAD/MI access token via
 * `WARM_FEED_TOKEN` (Bearer; the pipeline path), else an explicit/env PAT
 * (Basic; single-org, local), else an AAD token from `az account get-access-token`
 * (Bearer; local `az login`).
 */
export function resolveAuth(log: Logger, pat?: string): Auth {
  const bearer = process.env.WARM_FEED_TOKEN;
  if (bearer) {
    log.debug('using pre-acquired access token (WARM_FEED_TOKEN)');
    return {
      kind: 'aad',
      async header() {
        return {Authorization: `Bearer ${bearer}`};
      },
    };
  }

  const explicit = pat || envToken();
  if (explicit) {
    log.debug('using PAT auth');
    const basic = Buffer.from(`:${explicit}`).toString('base64');
    return {
      kind: 'pat',
      async header() {
        return {Authorization: `Basic ${basic}`};
      },
    };
  }

  let cached: string | undefined;
  const get = (): string => {
    if (!cached) {
      cached = azToken(log);
      if (!cached) {
        throw new Error(
          'No Azure DevOps auth available. Run `az login`, or set $env:ADO_PAT / pass --pat.',
        );
      }
      log.debug('using AAD auth from az');
    }
    return cached;
  };
  return {
    kind: 'aad',
    async header() {
      return {Authorization: `Bearer ${get()}`};
    },
  };
}
