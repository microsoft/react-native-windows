/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {fetchJson} from './http';
import type {Auth, Logger} from './types';

// --- npm ---------------------------------------------------------------------

interface NpmPackument {
  versions?: Record<string, {dist?: {tarball?: string}}>;
}

interface NpmInfo {
  versions: string[];
  tarballs: Record<string, string>;
}

export interface NpmRegistry {
  getVersions(id: string): Promise<string[]>;
  getTarballUrl(id: string, version: string): Promise<string | null>;
}

/** Encode a package id for a registry path, preserving the scope `@`. */
function encodeNpmId(id: string): string {
  return id.startsWith('@') ? id.replace('/', '%2F') : id;
}

export function createNpmRegistry(
  registry: string,
  auth: Auth,
  log: Logger,
): NpmRegistry {
  const base = registry.endsWith('/') ? registry : `${registry}/`;
  // Cache a compact {versions, tarballs} instead of the full packument so a feed
  // with thousands of packages does not hold every packument in memory.
  const cache = new Map<string, NpmInfo | null>();

  async function getInfo(id: string): Promise<NpmInfo | null> {
    const cached = cache.get(id);
    if (cached !== undefined) return cached;
    const headers = await auth.header();
    const {status, body} = await fetchJson<NpmPackument>(
      `${base}${encodeNpmId(id)}`,
      {...headers, Accept: 'application/vnd.npm.install-v1+json'},
    );
    if (status === 401 || status === 403) {
      throw new Error(`npm registry auth failed (${status}) for ${id}`);
    }
    if (!body || !body.versions) {
      log.debug(`npm packument ${id} -> ${status}`);
      cache.set(id, null);
      return null;
    }
    const versions = Object.keys(body.versions);
    const tarballs: Record<string, string> = {};
    for (const v of versions) {
      const url = body.versions[v].dist?.tarball;
      if (url) tarballs[v] = url;
    }
    const info: NpmInfo = {versions, tarballs};
    cache.set(id, info);
    return info;
  }

  return {
    async getVersions(id) {
      return (await getInfo(id))?.versions ?? [];
    },
    async getTarballUrl(id, version) {
      return (await getInfo(id))?.tarballs[version] ?? null;
    },
  };
}

// --- NuGet -------------------------------------------------------------------

interface ServiceIndex {
  resources?: Array<{'@id': string; '@type': string}>;
}

export interface NuGetRegistry {
  getVersions(id: string): Promise<string[]>;
  nupkgUrl(id: string, version: string): Promise<string>;
}

function pickResource(index: ServiceIndex, type: string): string | null {
  const r = index.resources?.find(x => x['@type'] === type);
  return r ? r['@id'] : null;
}

export function createNuGetRegistry(
  indexUrl: string,
  auth: Auth,
  _log: Logger,
): NuGetRegistry {
  let baseP: Promise<string> | null = null;
  const versionCache = new Map<string, string[]>();

  async function getBase(): Promise<string> {
    if (!baseP) {
      baseP = (async () => {
        const headers = await auth.header();
        const {status, body} = await fetchJson<ServiceIndex>(indexUrl, headers);
        if (!body)
          throw new Error(`NuGet service index ${indexUrl} -> ${status}`);
        const base =
          pickResource(body, 'PackageBaseAddress/3.0.0') ??
          pickResource(body, 'PackageBaseAddress/3.0.0-beta');
        if (!base)
          throw new Error('NuGet feed has no PackageBaseAddress resource');
        return base.endsWith('/') ? base : `${base}/`;
      })();
    }
    return baseP;
  }

  return {
    async getVersions(id) {
      const key = id.toLowerCase();
      const cached = versionCache.get(key);
      if (cached) return cached;
      const base = await getBase();
      const headers = await auth.header();
      const {status, body} = await fetchJson<{versions?: string[]}>(
        `${base}${key}/index.json`,
        headers,
      );
      if (status === 401 || status === 403) {
        throw new Error(`NuGet feed auth failed (${status}) for ${id}`);
      }
      const versions = body?.versions ?? [];
      versionCache.set(key, versions);
      return versions;
    },
    async nupkgUrl(id, version) {
      const base = await getBase();
      const k = id.toLowerCase();
      const v = version.toLowerCase();
      return `${base}${k}/${v}/${k}.${v}.nupkg`;
    },
  };
}
