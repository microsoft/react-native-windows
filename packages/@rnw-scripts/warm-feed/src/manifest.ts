/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Read a package.json into a flat set of npm dependency specs (`name -> range`)
 * for closure resolution. Only registry-resolvable ranges are kept: `workspace:`,
 * `link:`, `file:`, `portal:`, `patch:`, VCS/URL specs, and `user/repo` GitHub
 * shorthands are dropped (npm resolves those locally / from a URL, so they aren't
 * feed packages to warm).
 *
 * @format
 */

import {readFileSync} from 'node:fs';

// Ordered lowest -> highest precedence for the later-wins merge in manifestSpecs.
// peerDependencies (a broad constraint) is weakest; optionalDependencies is
// strongest because npm lets a same-name optional entry override the one in
// dependencies.
const DEP_SECTIONS = [
  'peerDependencies',
  'devDependencies',
  'dependencies',
  'optionalDependencies',
] as const;

/** True when `spec` is a range npm resolves from a registry (vs. local/VCS/URL). */
export function isResolvableSpec(spec: string): boolean {
  const s = spec.trim();
  if (!s) return false;
  if (
    /^(workspace|link|file|portal|patch|git\+ssh|git\+https|git|ssh|https?):/i.test(
      s,
    )
  ) {
    return false;
  }
  // A GitHub shorthand ('user/repo', 'user/repo#ref') contains '/'; a real range
  // never does. Keep `npm:` aliases (still registry-resolvable).
  if (s.includes('/') && !s.startsWith('npm:')) return false;
  return true;
}

/** Keep only the resolvable entries of a `name -> spec` object. */
export function sanitizeSpecs(
  deps: Record<string, unknown> | undefined,
): Record<string, string> {
  const out: Record<string, string> = {};
  if (!deps || typeof deps !== 'object') return out;
  for (const [name, spec] of Object.entries(deps)) {
    if (typeof spec === 'string' && isResolvableSpec(spec)) out[name] = spec;
  }
  return out;
}

/**
 * Merge a parsed package.json's dependency sections into one sanitized spec map.
 * Later sections win on a name clash, so a concrete dependency/devDependency
 * overrides a broad peerDependencies range for the same name (see DEP_SECTIONS).
 */
export function manifestSpecs(pkg: unknown): Record<string, string> {
  const p = (pkg ?? {}) as Record<string, Record<string, unknown> | undefined>;
  const out: Record<string, string> = {};
  for (const section of DEP_SECTIONS) {
    Object.assign(out, sanitizeSpecs(p[section]));
  }
  return out;
}

/** Read a package.json file and return its sanitized external dependency specs. */
export function readManifestSpecs(path: string): Record<string, string> {
  let pkg: unknown;
  try {
    pkg = JSON.parse(readFileSync(path, 'utf8'));
  } catch (err) {
    throw new Error(
      `failed to read manifest '${path}': ${(err as Error).message}`,
    );
  }
  return manifestSpecs(pkg);
}
