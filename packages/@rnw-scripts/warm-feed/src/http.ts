/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Azure Artifacts blocks upstream ingestion for requests it thinks come from a
 * browser. Node's global fetch (undici) auto-injects `Sec-Fetch-*` headers that
 * trip that check, so all feed requests go through node:https, which sends only
 * the headers we specify.
 *
 * @format
 */

import https from 'node:https';
import {URL} from 'node:url';

export function sleep(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}

const CLIENT_UA = 'warm-feed/0.1';
const DEFAULT_RETRY = [408, 429, 500, 502, 503, 504];
const REDIRECTS = new Set([301, 302, 303, 307, 308]);

export interface RawResponse {
  status: number;
  body: string | null;
}

interface RawOptions {
  method?: string;
  headers?: Record<string, string>;
  /** false = drain and discard the body (for warm GETs). */
  buffer?: boolean;
}

function rawOnce(
  url: string,
  opts: RawOptions,
  redirectsLeft: number,
): Promise<RawResponse> {
  return new Promise((resolve, reject) => {
    const headers: Record<string, string> = {
      'User-Agent': CLIENT_UA,
      ...(opts.headers ?? {}),
    };
    const req = https.request(
      new URL(url),
      {method: opts.method ?? 'GET', headers},
      res => {
        const status = res.statusCode ?? 0;
        const location = res.headers.location;
        if (REDIRECTS.has(status) && location && redirectsLeft > 0) {
          res.resume();
          const nextUrl = new URL(location, url);
          const cur = new URL(url);
          // Drop auth on cross-origin redirects (e.g. feed -> blob storage with
          // its own SAS); a stale bearer token would yield 403.
          let nextHeaders = opts.headers;
          if (nextUrl.host !== cur.host || nextUrl.protocol !== cur.protocol) {
            nextHeaders = {...(opts.headers ?? {})};
            delete nextHeaders.Authorization;
            delete nextHeaders.authorization;
          }
          rawOnce(
            nextUrl.toString(),
            {...opts, headers: nextHeaders},
            redirectsLeft - 1,
          ).then(resolve, reject);
          return;
        }
        if (opts.buffer === false) {
          res.resume();
          res.on('end', () => resolve({status, body: null}));
          res.on('error', reject);
          return;
        }
        const chunks: Buffer[] = [];
        res.on('data', (c: Buffer) => chunks.push(c));
        res.on('end', () =>
          resolve({status, body: Buffer.concat(chunks).toString('utf8')}),
        );
        res.on('error', reject);
      },
    );
    req.on('error', reject);
    req.end();
  });
}

export async function httpRequest(
  url: string,
  opts: RawOptions & {attempts?: number; retryStatuses?: number[]} = {},
): Promise<RawResponse> {
  const attempts = opts.attempts ?? 4;
  const retry = new Set(opts.retryStatuses ?? DEFAULT_RETRY);
  let lastError: unknown;
  let lastResponse: RawResponse | undefined;
  for (let i = 1; i <= attempts; i++) {
    try {
      const res = await rawOnce(url, opts, 5);
      if (res.status < 400 || !retry.has(res.status)) return res;
      lastResponse = res;
    } catch (err) {
      lastError = err;
    }
    if (i < attempts) {
      await sleep(500 * 2 ** (i - 1) + Math.floor(Math.random() * 250));
    }
  }
  if (lastResponse) return lastResponse;
  throw lastError ?? new Error(`request failed for ${url}`);
}

export async function fetchJson<T>(
  url: string,
  headers: Record<string, string>,
  attempts = 4,
): Promise<{status: number; body: T | null}> {
  const res = await httpRequest(url, {
    headers: {Accept: 'application/json', ...headers},
    attempts,
  });
  if (res.status >= 400 || res.body === null) {
    return {status: res.status, body: null};
  }
  try {
    return {status: res.status, body: JSON.parse(res.body) as T};
  } catch {
    return {status: res.status, body: null};
  }
}

/** GET a URL to trigger the feed's upstream save, draining and discarding the body. */
export async function warmGet(
  url: string,
  headers: Record<string, string>,
): Promise<number> {
  const res = await httpRequest(url, {
    headers,
    buffer: false,
    attempts: 5,
    // 404 while the feed is still pulling the version from upstream is retryable.
    retryStatuses: [404, 408, 429, 500, 502, 503, 504],
  });
  return res.status;
}
