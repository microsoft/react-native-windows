/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * NuGet closure warming from `packages.lock.json`. NuGet enumeration only refreshes
 * lines already in the feed, so a brand-new NuGet package name (or an exact
 * non-latest version a project's lock pins) would 404 under network isolation. The
 * repo's committed lock files list the full resolved closure (incl. transitives),
 * so warming every `name@resolved` they pin keeps isolated restores working —
 * mirroring the retired Warm-RnwFeedCache.ps1 NuGet pass.
 *
 * @format
 */

import {readdirSync, readFileSync} from 'node:fs';
import {join} from 'node:path';
import type {Logger, WarmTarget} from './types';

interface LockDep {
  type?: string;
  resolved?: string;
}

/**
 * Extract each `name@resolved` a NuGet `packages.lock.json` pins, across every
 * target framework, deduped. Skips project references (`type: Project`) and any
 * entry without a resolved version (a `Project` or a `CentralTransitive` stub).
 */
export function parseNuGetLock(
  lock: unknown,
): Array<{id: string; version: string}> {
  const deps = (lock as {dependencies?: unknown} | null)?.dependencies;
  if (!deps || typeof deps !== 'object') return [];
  const out = new Map<string, {id: string; version: string}>();
  for (const framework of Object.values(deps as Record<string, unknown>)) {
    if (!framework || typeof framework !== 'object') continue;
    for (const [id, info] of Object.entries(
      framework as Record<string, unknown>,
    )) {
      if (!info || typeof info !== 'object') continue;
      const dep = info as LockDep;
      if (dep.type === 'Project') continue;
      const version = dep.resolved;
      if (typeof version !== 'string' || !version) continue;
      out.set(`${id.toLowerCase()}|${version}`, {id, version});
    }
  }
  return [...out.values()];
}

/** Recursively find `packages.lock.json` under `root`, pruning node_modules/.git. */
function findLockFiles(root: string): string[] {
  const out: string[] = [];
  const stack = [root];
  while (stack.length > 0) {
    const dir = stack.pop();
    if (dir === undefined) continue;
    let entries;
    try {
      entries = readdirSync(dir, {withFileTypes: true});
    } catch {
      continue; // an unreadable dir (e.g. a broken symlink) is not fatal
    }
    for (const e of entries) {
      if (e.isDirectory()) {
        if (e.name === 'node_modules' || e.name === '.git') continue;
        stack.push(join(dir, e.name));
      } else if (e.isFile() && e.name === 'packages.lock.json') {
        out.push(join(dir, e.name));
      }
    }
  }
  return out;
}

/**
 * Scan `roots` for `packages.lock.json` files and turn every resolved NuGet
 * package they pin into a warm target (deduped across files).
 */
export function collectNuGetLockTargets(
  roots: string[],
  log: Logger,
): WarmTarget[] {
  const seen = new Map<string, WarmTarget>();
  for (const root of roots) {
    for (const file of findLockFiles(root)) {
      let json: unknown;
      try {
        json = JSON.parse(readFileSync(file, 'utf8'));
      } catch (err) {
        log.warn(`skipping ${file}: ${(err as Error).message}`);
        continue;
      }
      for (const {id, version} of parseNuGetLock(json)) {
        const key = `${id.toLowerCase()}|${version}`;
        if (!seen.has(key)) {
          seen.set(key, {
            ecosystem: 'nuget',
            id,
            version,
            source: 'nuget-lock',
          });
        }
      }
    }
  }
  return [...seen.values()];
}
