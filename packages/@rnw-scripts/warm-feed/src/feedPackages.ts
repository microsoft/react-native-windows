/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {fetchJson} from './http';
import type {Auth, Ecosystem, FeedPackage, Logger, WarmerConfig} from './types';

interface AdoPackageVersion {
  version?: string;
  isDeleted?: boolean;
}

interface AdoPackage {
  name?: string;
  versions?: AdoPackageVersion[];
}

interface AdoPackagesPage {
  value?: AdoPackage[];
}

const PROTOCOL: Record<Ecosystem, string> = {npm: 'Npm', nuget: 'NuGet'};

/**
 * List every package the feed already has (published + saved-from-upstream) via
 * the ADO Get Packages API, with each package's saved versions. This is both the
 * enumeration seed and the authoritative "already cached" set.
 */
export async function enumerateFeed(
  config: WarmerConfig,
  ecosystem: Ecosystem,
  auth: Auth,
  log: Logger,
): Promise<FeedPackage[]> {
  const {feedManagementBase, apiVersion, pageSize} = config.enumerate;
  const base = feedManagementBase.replace(/\/$/, '');
  const out: FeedPackage[] = [];
  let skip = 0;
  for (;;) {
    const url =
      `${base}/packages?api-version=${apiVersion}` +
      `&protocolType=${PROTOCOL[ecosystem]}&includeAllVersions=true` +
      `&$top=${pageSize}&$skip=${skip}`;
    const headers = await auth.header();
    const {status, body} = await fetchJson<AdoPackagesPage>(url, headers);
    if (status === 401 || status === 403) {
      throw new Error(`Get Packages auth failed (${status}) for ${ecosystem}`);
    }
    // A feed error or a wrong-shape page (no `value` array) must not be read as
    // the end of enumeration: that would exit successfully having warmed nothing.
    if (status < 200 || status >= 300 || !body || !Array.isArray(body.value)) {
      throw new Error(
        `Get Packages failed (status ${status}) for ${ecosystem}`,
      );
    }
    const page = body.value;
    if (page.length === 0) break;
    for (const p of page) {
      if (!p.name) continue;
      const savedVersions = (p.versions ?? [])
        .filter(v => v.version && !v.isDeleted)
        .map(v => v.version as string);
      out.push({ecosystem, id: p.name, savedVersions});
    }
    if (page.length < pageSize) break;
    skip += pageSize;
  }
  log.info(`enumerated ${out.length} ${ecosystem} package(s) from feed`);
  return out;
}
