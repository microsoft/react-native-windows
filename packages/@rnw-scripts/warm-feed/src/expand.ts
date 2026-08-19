/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {Ecosystem, ExpandConfig, FeedPackage, WarmTarget} from './types';
import type {NpmRegistry, NuGetRegistry} from './registries';
import {
  compareNuGet,
  compareSemver,
  isStable,
  isStableNuGet,
  parseNuGet,
  parseSemver,
  type NuGetVersion,
  type SemVer,
} from './versions';

/**
 * Reduce the in-use (major, minor) pairs to the set of lines to warm. For
 * 'in-use-majors' the minor is collapsed to `undefined` (match any minor).
 * `maxMajorsBack` keeps only the N most-recent majors.
 */
function inUseLines(
  pairs: ReadonlyArray<readonly [number, number]>,
  cfg: ExpandConfig,
): Array<[number, number | undefined]> {
  const majors = [...new Set(pairs.map(([maj]) => maj))].sort((a, b) => b - a);
  const allowed =
    cfg.maxMajorsBack > 0 ? new Set(majors.slice(0, cfg.maxMajorsBack)) : null;
  const seen = new Set<string>();
  const lines: Array<[number, number | undefined]> = [];
  for (const [maj, min] of pairs) {
    if (allowed && !allowed.has(maj)) continue;
    const key = cfg.scope === 'in-use-majors' ? `${maj}` : `${maj}.${min}`;
    if (seen.has(key)) continue;
    seen.add(key);
    lines.push([maj, cfg.scope === 'in-use-majors' ? undefined : min]);
  }
  return lines;
}

async function expandNpm(
  pkg: FeedPackage,
  npm: NpmRegistry,
  cfg: ExpandConfig,
): Promise<WarmTarget[]> {
  const saved = pkg.savedVersions
    .map(parseSemver)
    .filter((v): v is SemVer => v !== null)
    .filter(v => cfg.includePrerelease || isStable(v));
  if (saved.length === 0) return [];

  const pool = (await npm.getVersions(pkg.id))
    .map(parseSemver)
    .filter((v): v is SemVer => v !== null)
    .filter(v => cfg.includePrerelease || isStable(v));
  if (pool.length === 0) return [];

  const out = new Map<string, WarmTarget>();
  const add = (v: SemVer | undefined, source: string) => {
    if (v) out.set(v.raw, {ecosystem: 'npm', id: pkg.id, version: v.raw, source});
  };

  for (const [maj, min] of inUseLines(
    saved.map(v => [v.major, v.minor] as const),
    cfg,
  )) {
    let best: SemVer | undefined;
    for (const v of pool) {
      const match =
        min === undefined ? v.major === maj : v.major === maj && v.minor === min;
      if (match && (!best || compareSemver(v, best) > 0)) best = v;
    }
    add(best, 'expand:line');
  }
  if (cfg.includeLatest) {
    let latest: SemVer | undefined;
    for (const v of pool) if (!latest || compareSemver(v, latest) > 0) latest = v;
    add(latest, 'expand:latest');
  }
  return [...out.values()];
}

async function expandNuGet(
  pkg: FeedPackage,
  nuget: NuGetRegistry,
  cfg: ExpandConfig,
): Promise<WarmTarget[]> {
  const saved = pkg.savedVersions
    .map(parseNuGet)
    .filter((v): v is NuGetVersion => v !== null)
    .filter(v => cfg.includePrerelease || isStableNuGet(v));
  if (saved.length === 0) return [];

  const pool = (await nuget.getVersions(pkg.id))
    .map(parseNuGet)
    .filter((v): v is NuGetVersion => v !== null)
    .filter(v => cfg.includePrerelease || isStableNuGet(v));
  if (pool.length === 0) return [];

  const out = new Map<string, WarmTarget>();
  const add = (v: NuGetVersion | undefined, source: string) => {
    if (v) out.set(v.raw, {ecosystem: 'nuget', id: pkg.id, version: v.raw, source});
  };

  for (const [maj, min] of inUseLines(
    saved.map(v => [v.parts[0], v.parts[1]] as const),
    cfg,
  )) {
    let best: NuGetVersion | undefined;
    for (const v of pool) {
      const match =
        min === undefined
          ? v.parts[0] === maj
          : v.parts[0] === maj && v.parts[1] === min;
      if (match && (!best || compareNuGet(v, best) > 0)) best = v;
    }
    add(best, 'expand:line');
  }
  if (cfg.includeLatest) {
    let latest: NuGetVersion | undefined;
    for (const v of pool) if (!latest || compareNuGet(v, latest) > 0) latest = v;
    add(latest, 'expand:latest');
  }
  return [...out.values()];
}

/** Compute the warm targets (latest patch per in-use line) for one feed package. */
export function expandPackage(
  ecosystem: Ecosystem,
  pkg: FeedPackage,
  registry: NpmRegistry | NuGetRegistry,
  cfg: ExpandConfig,
): Promise<WarmTarget[]> {
  return ecosystem === 'npm'
    ? expandNpm(pkg, registry as NpmRegistry, cfg)
    : expandNuGet(pkg, registry as NuGetRegistry, cfg);
}
