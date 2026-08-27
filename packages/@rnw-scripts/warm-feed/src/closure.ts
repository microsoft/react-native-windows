/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Closure warming: resolve a set of npm dependency specs to their full
 * transitive graph and turn each resolved `name@version` into a warm target.
 *
 * The graph comes from `npm install --package-lock-only` (metadata only, no
 * tarball downloads); the feed save happens through `warmGet` on each target.
 *
 * The lockfile covers the resolve host's own platform (Windows x64 in CI). Other
 * OS/CPU optional variants are not captured dependably against an Azure Artifacts
 * upstream proxy — npm 11+ writes not-yet-cached ones without a `version`
 * (npm/cli#9342) and `parseNpmLock` drops version-less entries — so full
 * cross-platform warming would need a per-OS/CPU resolve.
 *
 * @format
 */

import {spawnSync} from 'node:child_process';
import {
  existsSync,
  mkdtempSync,
  readFileSync,
  rmSync,
  writeFileSync,
} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {URL} from 'node:url';
import type {Auth, Ctx, WarmTarget} from './types';
import {parseSemver} from './versions';

/** A named set of npm dependency specs (`name -> range`) to resolve and warm. */
export interface DepSpecSet {
  label: string;
  specs: Record<string, string>;
}

interface ResolveOptions {
  /** npm registry (the feed) to resolve against. */
  registry: string;
  /** Path to an authenticated `.npmrc` for the resolve. */
  npmrcPath: string;
  /** Diagnostic label for logs / the target `source`. */
  label: string;
}

/** True for an Azure DevOps Artifacts feed host — the only host the token is valid for. */
function isAzureDevOpsFeed(url: string): boolean {
  let host: string;
  try {
    host = new URL(url).host.toLowerCase();
  } catch {
    return false;
  }
  return (
    host === 'pkgs.dev.azure.com' || host.endsWith('.pkgs.visualstudio.com')
  );
}

/**
 * Write an `.npmrc` for npm/npx into `dir`; returns its path. The feed credential
 * is attached only when the registry is an Azure DevOps host, so a custom
 * `closure.registry` can't leak the token to npmjs or another foreign registry.
 */
export async function writeFeedNpmrc(
  auth: Auth,
  registryUrl: string,
  dir: string,
): Promise<string> {
  const lines = [`registry=${registryUrl}`];
  if (isAzureDevOpsFeed(registryUrl)) {
    const token = await auth.token();
    // Azure Artifacts keys the token by the registry URL without its scheme.
    const key = registryUrl.replace(/^https?:/i, '');
    lines.push(`${key}:_authToken=${token}`);
  }
  const p = join(dir, '.npmrc');
  writeFileSync(p, `${lines.join('\n')}\n`, {encoding: 'utf8'});
  return p;
}

/**
 * Run a Node CLI (npm/npx) with feed config injected via `npm_config_*` env vars
 * (so no registry/userconfig paths land on the command line, avoiding Windows
 * quoting). `shell` is required to launch the `.cmd` shims on Windows.
 */
export function runTool(
  cmd: string,
  args: string[],
  cwd: string,
  extraEnv: Record<string, string>,
  label: string,
): string {
  const res = spawnSync(cmd, args, {
    cwd,
    encoding: 'utf8',
    env: {...process.env, ...extraEnv},
    shell: true,
    maxBuffer: 128 * 1024 * 1024,
  });
  if (res.error) {
    throw new Error(`${label}: ${res.error.message}`);
  }
  if (res.status !== 0) {
    const out = `${res.stdout}${res.stderr}`.trim();
    throw new Error(
      `${label} failed (exit ${res.status ?? 'null'}): ${out.slice(0, 4000)}`,
    );
  }
  return res.stdout;
}

/** Last `node_modules/<name>` segment of a lockfile v2/v3 `packages` key. */
function nameFromLockKey(key: string): string | null {
  const marker = 'node_modules/';
  const i = key.lastIndexOf(marker);
  if (i < 0) return null;
  const name = key.slice(i + marker.length);
  return name.length > 0 ? name : null;
}

interface LockEntry {
  name?: string;
  version?: string;
  resolved?: string;
  link?: boolean;
}

function keepEntry(id: string | null, e: LockEntry): id is string {
  if (!id || !e.version) return false;
  if (e.link) return false; // a workspace/local link, not a registry package
  // A non-http `resolved` (file:/git:/etc.) is not a feed-warmable package.
  if (e.resolved && !/^https?:/i.test(e.resolved)) return false;
  return parseSemver(e.version) !== null;
}

/**
 * Extract every registry `name@version` from an npm `package-lock.json`. Supports
 * the v2/v3 `packages` map (preferred) and falls back to the v1 `dependencies`
 * tree. Local links, workspaces, and non-registry (file/git/url) specs are skipped.
 */
export function parseNpmLock(
  lock: unknown,
): Array<{id: string; version: string}> {
  const out = new Map<string, {id: string; version: string}>();
  const add = (id: string, version: string) => {
    out.set(`${id.toLowerCase()}@${version}`, {id, version});
  };

  const root = lock as {
    packages?: Record<string, unknown>;
    dependencies?: Record<string, unknown>;
  } | null;

  const packages = root?.packages;
  if (packages && typeof packages === 'object') {
    for (const [key, val] of Object.entries(packages)) {
      if (key === '' || !val || typeof val !== 'object') continue;
      const e = val as LockEntry;
      // Prefer the entry's real name: an alias install keeps the alias in the path
      // key (e.g. node_modules/string-width-cjs) and the true package in `name`.
      const id = e.name ?? nameFromLockKey(key);
      if (keepEntry(id, e)) add(id, e.version as string);
    }
  }

  // v1 fallback: a nested `dependencies` tree keyed by package name.
  const walk = (deps: Record<string, unknown>) => {
    for (const [key, val] of Object.entries(deps)) {
      if (!val || typeof val !== 'object') continue;
      const e = val as LockEntry & {dependencies?: unknown};
      const id = e.name ?? key;
      if (keepEntry(id, e)) add(id, e.version as string);
      const nested = e.dependencies;
      if (nested && typeof nested === 'object') {
        walk(nested as Record<string, unknown>);
      }
    }
  };
  if (out.size === 0 && root?.dependencies) walk(root.dependencies);

  return [...out.values()];
}

/**
 * Resolve `specs` to their full npm graph and return one warm target per resolved
 * `name@version`. Runs in a throwaway temp dir that is always cleaned up.
 */
export async function resolveClosureSpecs(
  ctx: Ctx,
  specs: Record<string, string>,
  opts: ResolveOptions,
): Promise<WarmTarget[]> {
  const names = Object.keys(specs);
  if (names.length === 0) {
    ctx.log.warn(`closure ${opts.label}: no dependencies to resolve`);
    return [];
  }
  const dir = mkdtempSync(join(tmpdir(), 'warm-closure-'));
  try {
    writeFileSync(
      join(dir, 'package.json'),
      JSON.stringify(
        {
          name: 'warm-feed-closure',
          version: '0.0.0',
          private: true,
          dependencies: specs,
        },
        null,
        2,
      ),
    );
    ctx.log.info(
      `closure ${opts.label}: resolving ${names.length} root dep(s) via npm --package-lock-only`,
    );
    runTool(
      'npm',
      ['install'],
      dir,
      {
        npm_config_userconfig: opts.npmrcPath,
        npm_config_registry: opts.registry,
        npm_config_package_lock_only: 'true',
        npm_config_legacy_peer_deps: 'true',
        npm_config_audit: 'false',
        npm_config_fund: 'false',
        npm_config_ignore_scripts: 'true',
      },
      `npm resolve (${opts.label})`,
    );
    const lockPath = join(dir, 'package-lock.json');
    if (!existsSync(lockPath)) {
      throw new Error(`closure ${opts.label}: npm produced no package-lock.json`);
    }
    const entries = parseNpmLock(JSON.parse(readFileSync(lockPath, 'utf8')));
    ctx.log.info(
      `closure ${opts.label}: ${entries.length} package version(s) in the graph`,
    );
    return entries.map(e => ({
      ecosystem: 'npm' as const,
      id: e.id,
      version: e.version,
      source: `closure:${opts.label}`,
    }));
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
}
