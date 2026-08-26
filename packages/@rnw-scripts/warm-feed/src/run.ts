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
  WarmerConfig,
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
import {mkdtempSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {basename, join} from 'node:path';
import {resolveClosureSpecs, writeFeedNpmrc} from './closure';
import {readManifestSpecs} from './manifest';
import {
  getSpecialModule,
  specialModuleNames,
  type SpecialModuleContext,
} from './specialModules';

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

export function parseSpec(
  spec: string,
  warn: (m: string) => void,
): WarmTarget | null {
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
  // Treat every non-warmed result (missing or failed) as a failed run: a version
  // that still 404s after all retries was not actually cached.
  const notWarmed = results.filter(r => r.status !== 'warmed');
  for (const f of notWarmed.slice(0, 20)) {
    log.error(
      `${f.status.toUpperCase()} ${f.target.ecosystem} ${f.target.id}@${f.target.version}: ${f.detail}`,
    );
  }
  return notWarmed.length > 0 ? 1 : 0;
}

function specsFromRoots(
  roots: string[],
  warn: (m: string) => void,
): Record<string, string> {
  const specs: Record<string, string> = {};
  const flat = roots.flatMap(r => r.split(/\s+/)).filter(Boolean);
  for (const s of flat) {
    const t = parseSpec(s, warn);
    if (!t) continue;
    if (t.ecosystem !== 'npm') {
      warn(`closure roots are npm-only, ignoring '${s}'`);
      continue;
    }
    specs[t.id] = t.version;
  }
  return specs;
}

function enabledClosureModules(config: WarmerConfig): string[] {
  return Object.entries(config.closure.modules)
    .filter(([, c]) => c.enabled !== false)
    .map(([name]) => name);
}

/**
 * Resolve the npm closures requested by roots (--closure), manifests
 * (--closure-manifest), and special modules, into a deduped warm-target list. All
 * npm/npx subprocesses share one authenticated feed `.npmrc` in a temp dir that is
 * always cleaned up.
 */
async function collectClosureTargets(
  ctx: Ctx,
  registries: Registries,
  moduleNames: string[],
): Promise<WarmTarget[]> {
  const {config, options, log} = ctx;
  const registryUrl = config.closure.registry ?? config.feeds.npm?.registry;
  if (!registryUrl) {
    throw new Error(
      'closure warming requires feeds.npm.registry (or closure.registry)',
    );
  }
  if (moduleNames.length > 0 && !registries.npm) {
    throw new Error('closure modules require an npm feed (feeds.npm.registry)');
  }

  const targets: WarmTarget[] = [];
  const tmp = mkdtempSync(join(tmpdir(), 'warm-feed-npmrc-'));
  try {
    const npmrcPath = await writeFeedNpmrc(ctx.auth, registryUrl, tmp);
    const opt = (label: string) => ({registry: registryUrl, npmrcPath, label});

    const rootSpecs = specsFromRoots(options.closureRoots, m => log.warn(m));
    if (Object.keys(rootSpecs).length > 0) {
      targets.push(...(await resolveClosureSpecs(ctx, rootSpecs, opt('roots'))));
    }

    for (const manifest of options.closureManifests) {
      targets.push(
        ...(await resolveClosureSpecs(
          ctx,
          readManifestSpecs(manifest),
          opt(`manifest:${basename(manifest)}`),
        )),
      );
    }

    if (moduleNames.length > 0) {
      const mctx: SpecialModuleContext = {
        ctx,
        repoRoot: options.repoRoot ?? process.cwd(),
        registry: registries.npm!,
        npmRegistryUrl: registryUrl,
        npmrcPath,
      };
      for (const name of moduleNames) {
        const mod = getSpecialModule(name);
        if (!mod) {
          log.error(
            `unknown closure module '${name}' (known: ${
              specialModuleNames().join(', ') || 'none'
            })`,
          );
          continue;
        }
        const sets = await mod.collectDepSpecs(
          mctx,
          config.closure.modules[name] ?? {},
        );
        for (const set of sets) {
          targets.push(
            ...(await resolveClosureSpecs(ctx, set.specs, opt(set.label))),
          );
        }
      }
    }
    return dedupe(targets);
  } finally {
    rmSync(tmp, {recursive: true, force: true});
  }
}

type KeepFn = (t: WarmTarget) => boolean;

async function runOneOffPackages(
  ctx: Ctx,
  registries: Registries,
  only: Ecosystem | undefined,
  keep: KeepFn,
): Promise<number> {
  const {options, log} = ctx;
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

async function runOneOffClosure(
  ctx: Ctx,
  registries: Registries,
): Promise<number> {
  const {options, config, log} = ctx;
  const moduleNames = options.closureModules.includes('all')
    ? enabledClosureModules(config)
    : options.closureModules;
  const targets = await collectClosureTargets(ctx, registries, moduleNames);
  if (options.dryRun) {
    log.info(`dry run (closure): ${targets.length} target(s)`);
    for (const t of targets.slice(0, 40)) {
      log.info(`  ${t.ecosystem} ${t.id}@${t.version} [${t.source}]`);
    }
    if (targets.length > 40) {
      log.info(`  ... and ${targets.length - 40} more`);
    }
    return 0;
  }
  return warmTargets(ctx, registries, targets);
}

async function runScheduled(
  ctx: Ctx,
  registries: Registries,
  only: Ecosystem | undefined,
  keep: KeepFn,
): Promise<number> {
  const {options, config, auth, log} = ctx;
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
          // Don't swallow: a systemic auth/feed failure would otherwise produce
          // an empty plan and a green run that warmed nothing.
          throw new Error(
            `expand ${eco} ${p.id} failed: ${(err as Error).message}`,
          );
        }
      },
    );
    for (const list of perPackage) targets.push(...list);
  }

  // Fold in the enabled special-module closures (npm only) so the schedule keeps
  // brand-new graphs (e.g. the CLI-init lib) warm alongside the latest-patch sync.
  // A closure failure is surfaced (non-zero exit) but never blocks the feed-wide
  // sync from warming what it enumerated.
  let closureFailed = false;
  const scheduledModules =
    only === 'nuget' ? [] : enabledClosureModules(config);
  if (scheduledModules.length > 0) {
    try {
      const closureTargets = await collectClosureTargets(
        ctx,
        registries,
        scheduledModules,
      );
      for (const t of closureTargets) targets.push(t);
    } catch (err) {
      log.error(`closure modules failed: ${(err as Error).message}`);
      closureFailed = true;
    }
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
    return closureFailed ? 1 : 0;
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
  const code = await warmTargets(ctx, registries, toWarm);
  return code !== 0 ? code : closureFailed ? 1 : 0;
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

  let only: Ecosystem | undefined;
  if (options.only === 'npm' || options.only === 'nuget') {
    only = options.only;
  } else if (options.only) {
    log.error(`invalid --only '${options.only}': expected 'npm' or 'nuget'`);
    return 2;
  }

  const ignoreRules = config.ignore
    .map(parseIgnore)
    .filter((r): r is IgnoreRule => r !== null);
  const keep: KeepFn = t =>
    ignoreRules.length === 0 || !matchesIgnore(t, ignoreRules);

  if (options.packages.length) {
    return runOneOffPackages(ctx, registries, only, keep);
  }

  const closureRequested =
    options.closureRoots.length > 0 ||
    options.closureManifests.length > 0 ||
    options.closureModules.length > 0;
  if (closureRequested) {
    return runOneOffClosure(ctx, registries);
  }

  return runScheduled(ctx, registries, only, keep);
}
