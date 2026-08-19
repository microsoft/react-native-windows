/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {
  Ctx,
  Ecosystem,
  FeedPackage,
  RunOptions,
  WarmResult,
  WarmTarget,
} from './types';
import {loadConfig} from './config';
import {resolveAuth} from './auth';
import {createLogger} from './logger';
import {
  createNpmRegistry,
  createNuGetRegistry,
  type NpmRegistry,
  type NuGetRegistry,
} from './registries';
import {enumerateFeed} from './feedPackages';
import {expandPackage} from './expand';
import {createWarmers} from './warmers';
import {pool} from './pool';

interface Registries {
  npm?: NpmRegistry;
  nuget?: NuGetRegistry;
}

interface IgnoreRule {
  ecosystem?: Ecosystem;
  id: string;
  version?: string;
}

function targetKey(t: WarmTarget): string {
  return `${t.ecosystem}|${t.id.toLowerCase()}|${t.version}`;
}

function savedKey(ecosystem: Ecosystem, id: string): string {
  return `${ecosystem}|${id.toLowerCase()}`;
}

function dedupe(targets: WarmTarget[]): WarmTarget[] {
  const map = new Map<string, WarmTarget>();
  for (const t of targets) {
    if (!map.has(targetKey(t))) map.set(targetKey(t), t);
  }
  return [...map.values()];
}

function parseIgnore(entry: string): IgnoreRule | null {
  let s = entry.trim();
  if (!s) return null;
  let ecosystem: Ecosystem | undefined;
  const eco = /^(npm|nuget):(.*)$/i.exec(s);
  if (eco) {
    ecosystem = eco[1].toLowerCase() as Ecosystem;
    s = eco[2];
  }
  const at = s.lastIndexOf('@');
  if (at > 0) return {ecosystem, id: s.slice(0, at), version: s.slice(at + 1)};
  return {ecosystem, id: s};
}

function matchesIgnore(t: WarmTarget, rules: IgnoreRule[]): boolean {
  return rules.some(
    r =>
      (!r.ecosystem || r.ecosystem === t.ecosystem) &&
      r.id.toLowerCase() === t.id.toLowerCase() &&
      (!r.version || r.version === '*' || r.version === t.version),
  );
}

function parseSpec(spec: string, warn: (m: string) => void): WarmTarget | null {
  const s = spec.trim();
  if (!s) return null;
  const m = /^(npm|nuget):(.+)$/i.exec(s);
  if (!m) {
    warn(`ignoring '${spec}': expected 'npm:id@version' or 'nuget:id@version'`);
    return null;
  }
  const rest = m[2];
  const at = rest.lastIndexOf('@');
  if (at <= 0) {
    warn(`ignoring '${spec}': missing @version`);
    return null;
  }
  return {
    ecosystem: m[1].toLowerCase() as Ecosystem,
    id: rest.slice(0, at),
    version: rest.slice(at + 1),
    source: 'cli-packages',
  };
}

function summarize(results: WarmResult[]): Record<string, number> {
  const counts: Record<string, number> = {warmed: 0, missing: 0, failed: 0};
  for (const r of results) counts[r.status] = (counts[r.status] ?? 0) + 1;
  return counts;
}

async function warmTargets(
  ctx: Ctx,
  registries: Registries,
  targets: WarmTarget[],
): Promise<number> {
  const {log} = ctx;
  if (targets.length === 0) {
    log.info('nothing to warm');
    return 0;
  }
  // Fail fast if no credentials are available before issuing warm requests.
  try {
    await ctx.auth.header();
  } catch (err) {
    log.error((err as Error).message);
    return 2;
  }
  const warmers = createWarmers(ctx, registries);
  const concurrency = ctx.options.concurrency ?? ctx.config.concurrency;
  log.info(`warming ${targets.length} target(s) (concurrency ${concurrency})`);
  const results = await pool(
    targets,
    concurrency,
    async (t): Promise<WarmResult> => {
      const warmer = warmers[t.ecosystem];
      if (!warmer) {
        return {target: t, status: 'failed', detail: 'no warmer for ecosystem'};
      }
      try {
        const r = await warmer.warm(t);
        if (r.status === 'warmed') {
          log.debug(`warmed ${t.ecosystem} ${t.id}@${t.version}`);
        } else if (r.status === 'missing') {
          log.warn(`missing ${t.ecosystem} ${t.id}@${t.version} (${r.detail})`);
        }
        return r;
      } catch (err) {
        return {target: t, status: 'failed', detail: (err as Error).message};
      }
    },
  );
  const counts = summarize(results);
  log.info(
    `done: warmed=${counts.warmed} missing=${counts.missing} failed=${counts.failed}`,
  );
  const failures = results.filter(r => r.status === 'failed');
  for (const f of failures.slice(0, 20)) {
    log.error(
      `FAILED ${f.target.ecosystem} ${f.target.id}@${f.target.version}: ${f.detail}`,
    );
  }
  return failures.length > 0 ? 1 : 0;
}

export async function run(options: RunOptions, pat?: string): Promise<number> {
  const log = createLogger(options.verbose);
  const config = loadConfig(options.configPath);
  const auth = resolveAuth(log, pat);
  const ctx: Ctx = {config, options, auth, log};

  const registries: Registries = {};
  if (config.feeds.npm?.registry) {
    registries.npm = createNpmRegistry(config.feeds.npm.registry, auth, log);
  }
  if (config.feeds.nuget?.index) {
    registries.nuget = createNuGetRegistry(config.feeds.nuget.index, auth, log);
  }

  let only = options.only;
  if (only && only !== 'npm' && only !== 'nuget') {
    log.warn(`ignoring invalid --only '${only}'`);
    only = undefined;
  }

  const ignoreRules = config.ignore
    .map(parseIgnore)
    .filter((r): r is IgnoreRule => r !== null);
  const keep = (t: WarmTarget) =>
    ignoreRules.length === 0 || !matchesIgnore(t, ignoreRules);

  // One-off mode: warm exactly the versions passed on the command line.
  if (options.packages.length) {
    const specs = options.packages.flatMap(p => p.split(/\s+/)).filter(Boolean);
    const targets = dedupe(
      specs
        .map(s => parseSpec(s, m => log.warn(m)))
        .filter((t): t is WarmTarget => t !== null)
        .filter(t => !only || t.ecosystem === only)
        .filter(keep),
    );
    if (options.dryRun) {
      log.info(`dry run (one-off): ${targets.length} target(s)`);
      for (const t of targets) {
        log.info(`  ${t.ecosystem} ${t.id}@${t.version} [${t.source}]`);
      }
      return 0;
    }
    log.info(`one-off warm: ${targets.length} target(s)`);
    return warmTargets(ctx, registries, targets);
  }

  // Scheduled mode: enumerate the feed, expand to latest patch per in-use line,
  // skip what is already cached, warm the rest.
  const ecosystems = (['npm', 'nuget'] as Ecosystem[])
    .filter(e => (e === 'npm' ? registries.npm : registries.nuget))
    .filter(e => !only || e === only);

  const savedByPackage = new Map<string, Set<string>>();
  const targets: WarmTarget[] = [];
  for (const eco of ecosystems) {
    const packages = await enumerateFeed(config, eco, auth, log);
    for (const p of packages) {
      savedByPackage.set(savedKey(eco, p.id), new Set(p.savedVersions));
    }
    const registry = eco === 'npm' ? registries.npm! : registries.nuget!;
    const perPackage = await pool(
      packages,
      config.concurrency,
      async (p: FeedPackage): Promise<WarmTarget[]> => {
        try {
          return await expandPackage(eco, p, registry, config.expand);
        } catch (err) {
          log.warn(`expand ${eco} ${p.id} failed: ${(err as Error).message}`);
          return [];
        }
      },
    );
    for (const list of perPackage) targets.push(...list);
  }

  const deduped = dedupe(targets.filter(keep));

  if (options.dryRun) {
    const byEco = deduped.reduce<Record<string, number>>((acc, t) => {
      acc[t.ecosystem] = (acc[t.ecosystem] ?? 0) + 1;
      return acc;
    }, {});
    log.info(`dry run: ${deduped.length} target(s) ${JSON.stringify(byEco)}`);
    for (const t of deduped.slice(0, 40)) {
      log.info(`  ${t.ecosystem} ${t.id}@${t.version} [${t.source}]`);
    }
    if (deduped.length > 40) log.info(`  ... and ${deduped.length - 40} more`);
    return 0;
  }

  const toWarm = options.verify
    ? deduped
    : deduped.filter(t => {
        const saved = savedByPackage.get(savedKey(t.ecosystem, t.id));
        return !saved || !saved.has(t.version);
      });
  log.info(
    `targets ${deduped.length}, already cached ${
      deduped.length - toWarm.length
    }, to warm ${toWarm.length}`,
  );
  return warmTargets(ctx, registries, toWarm);
}
