/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * create-react-native-library special module.
 *
 * The CLI-init lib test doesn't install cRNL's own dependencies — it scaffolds a
 * library (+ vanilla example app) with `create-react-native-library` and installs
 * *that generated project*. So to warm the versions the test needs, we reproduce
 * the scaffold and resolve the generated project's dependency closure.
 *
 * warm-feed runs only from `main`, but the test runs on every release branch, each
 * pinned to a different React Native. The config manifest therefore lists all
 * branches; per branch we derive the RN/CLI versions (nightly for `main` from the
 * working-tree vnext/package.json; latest stable `0.NN.x` for `0.NN-stable`),
 * scaffold, and read the generated manifests. Mirrors vnext/Scripts/creaternwlib.cmd.
 *
 * @format
 */

import {existsSync, mkdtempSync, readFileSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {runTool, type DepSpecSet} from '../closure';
import {manifestSpecs} from '../manifest';
import {compareSemver, isStable, parseSemver} from '../versions';
import type {SpecialModule, SpecialModuleContext} from './types';

interface CrnlBranch {
  /** Branch name, e.g. 'main' or '0.85-stable'. */
  name: string;
  /** `main` uses a nightly RN and needs the generated deps rewritten. */
  nightly?: boolean;
  /** Override the derived RN version passed to `--react-native-version`. */
  reactNativeVersion?: string;
  /** Override the CLI version used by the nightly rewrite. */
  reactNativeCliVersion?: string;
  /** Override the `react-native-windows` spec added to the library. */
  reactNativeWindowsSpec?: string;
}

interface CrnlConfig {
  createLibraryVersion: string;
  type: string;
  example: string;
  languages: string;
  tools: string[];
  branches: CrnlBranch[];
}

interface BranchVersions {
  reactNative: string;
  reactNativeCli?: string;
  reactNativeWindowsSpec?: string;
  nightly: boolean;
}

const DEFAULTS = {
  type: 'turbo-module',
  example: 'vanilla',
  languages: 'kotlin-objc',
  // cRNL 0.63 requires --tools; warm the superset so any subset the test picks is covered.
  tools: ['eslint', 'jest', 'lefthook', 'release-it', 'vite'],
};

function parseTools(value: unknown): string[] {
  if (
    Array.isArray(value) &&
    value.length > 0 &&
    value.every(t => typeof t === 'string')
  ) {
    return value as string[];
  }
  return DEFAULTS.tools;
}

export function parseCrnlConfig(config: Record<string, unknown>): CrnlConfig {
  const createLibraryVersion = config.createLibraryVersion;
  if (typeof createLibraryVersion !== 'string' || !createLibraryVersion) {
    throw new Error(
      "create-react-native-library module requires a 'createLibraryVersion'",
    );
  }
  const branchesRaw = config.branches;
  if (!Array.isArray(branchesRaw) || branchesRaw.length === 0) {
    throw new Error(
      "create-react-native-library module requires a non-empty 'branches' list",
    );
  }
  const branches = branchesRaw.map((b): CrnlBranch => {
    const rec = (b ?? {}) as Record<string, unknown>;
    if (typeof rec.name !== 'string' || !rec.name) {
      throw new Error("each cRNL branch requires a 'name'");
    }
    return {
      name: rec.name,
      nightly: rec.nightly === true,
      reactNativeVersion:
        typeof rec.reactNativeVersion === 'string'
          ? rec.reactNativeVersion
          : undefined,
      reactNativeCliVersion:
        typeof rec.reactNativeCliVersion === 'string'
          ? rec.reactNativeCliVersion
          : undefined,
      reactNativeWindowsSpec:
        typeof rec.reactNativeWindowsSpec === 'string'
          ? rec.reactNativeWindowsSpec
          : undefined,
    };
  });
  return {
    createLibraryVersion,
    type: typeof config.type === 'string' ? config.type : DEFAULTS.type,
    example:
      typeof config.example === 'string' ? config.example : DEFAULTS.example,
    languages:
      typeof config.languages === 'string'
        ? config.languages
        : DEFAULTS.languages,
    tools: parseTools(config.tools),
    branches,
  };
}

/** The minor N of a '0.N-stable' (or '0.N') branch name, else null. */
export function stableMinor(branchName: string): number | null {
  const m = /^v?0\.(\d+)(?:-stable)?$/.exec(branchName.trim());
  return m ? Number(m[1]) : null;
}

/**
 * Highest `0.<minor>.x` version of `id` in the feed. Prefers a stable release,
 * but falls back to the newest prerelease when the line is still preview-only
 * (e.g. react-native-windows 0.85 ships as 0.85.0-preview.N before a stable 0.85).
 */
async function latestLineVersion(
  mctx: SpecialModuleContext,
  id: string,
  minor: number,
): Promise<string | null> {
  const inLine = (await mctx.registry.getVersions(id))
    .map(parseSemver)
    .filter((v): v is NonNullable<typeof v> => v !== null)
    .filter(v => v.major === 0 && v.minor === minor);
  const stable = inLine.filter(isStable);
  const pool = stable.length > 0 ? stable : inLine;
  if (pool.length === 0) return null;
  return pool.reduce((a, b) => (compareSemver(a, b) >= 0 ? a : b)).raw;
}

function readVnextField(
  repoRoot: string,
  section: 'dependencies' | 'devDependencies',
  name: string,
): string | undefined {
  const pkgPath = join(repoRoot, 'vnext', 'package.json');
  const pkg = JSON.parse(readFileSync(pkgPath, 'utf8')) as Record<
    string,
    Record<string, string> | undefined
  >;
  return pkg[section]?.[name];
}

export async function resolveBranchVersions(
  mctx: SpecialModuleContext,
  branch: CrnlBranch,
): Promise<BranchVersions> {
  const minor = stableMinor(branch.name);

  let reactNative = branch.reactNativeVersion;
  let reactNativeCli = branch.reactNativeCliVersion;
  if (!reactNative) {
    if (branch.nightly) {
      reactNative = readVnextField(
        mctx.repoRoot,
        'devDependencies',
        'react-native',
      );
      reactNativeCli =
        reactNativeCli ??
        readVnextField(mctx.repoRoot, 'dependencies', '@react-native-community/cli');
    } else if (minor !== null) {
      reactNative =
        (await latestLineVersion(mctx, 'react-native', minor)) ?? undefined;
    }
  }
  if (!reactNative) {
    throw new Error(
      `cRNL: could not determine a react-native version for branch '${branch.name}' (set reactNativeVersion)`,
    );
  }

  // Pin the actual latest 0.NN.x react-native-windows (stable, or a preview for a
  // preview-only line); a `^0.NN.0` range would ETARGET when only previews exist.
  let reactNativeWindowsSpec = branch.reactNativeWindowsSpec;
  if (!reactNativeWindowsSpec && !branch.nightly && minor !== null) {
    reactNativeWindowsSpec =
      (await latestLineVersion(mctx, 'react-native-windows', minor)) ?? undefined;
  }

  return {
    reactNative,
    reactNativeCli,
    reactNativeWindowsSpec,
    nightly: branch.nightly === true,
  };
}

/**
 * Rewrite generated nightly deps to the nightly RN/CLI, mirroring
 * vnext/Scripts/UpdateNightlyDependencies.ps1 (cRNL can't pin nightly RN itself).
 */
export function nightlyFixupSpecs(
  specs: Record<string, string>,
  reactNativeVersion: string,
  reactNativeCliVersion?: string,
): void {
  for (const name of Object.keys(specs)) {
    if (name === 'react-native' && !specs[name].includes('*')) {
      specs[name] = reactNativeVersion;
    } else if (name === '@react-native/new-app-screen') {
      specs[name] = reactNativeVersion;
    } else if (/^@react-native\/.+-(config|preset)$/.test(name)) {
      specs[name] = reactNativeVersion;
    } else if (
      reactNativeCliVersion &&
      /^@react-native-community\/cli(?:-platform-(?:ios|android))?$/.test(name)
    ) {
      specs[name] = reactNativeCliVersion;
    }
  }
}

/** Merge the generated library + example manifests into one spec set. */
function collectScaffoldSpecs(
  projectDir: string,
  versions: BranchVersions,
): Record<string, string> {
  const specs = manifestSpecs(
    JSON.parse(readFileSync(join(projectDir, 'package.json'), 'utf8')),
  );
  const examplePkg = join(projectDir, 'example', 'package.json');
  if (existsSync(examplePkg)) {
    Object.assign(
      specs,
      manifestSpecs(JSON.parse(readFileSync(examplePkg, 'utf8'))),
    );
  }
  if (versions.nightly) {
    nightlyFixupSpecs(specs, versions.reactNative, versions.reactNativeCli);
  }
  if (versions.reactNativeWindowsSpec) {
    specs['react-native-windows'] = versions.reactNativeWindowsSpec;
  }
  return specs;
}

function scaffold(
  mctx: SpecialModuleContext,
  cfg: CrnlConfig,
  branch: CrnlBranch,
  versions: BranchVersions,
): {workDir: string; projectDir: string} {
  const workDir = mkdtempSync(join(tmpdir(), 'warm-crnl-'));
  // Space-free arg values so the Windows shell launch needs no quoting.
  const args = [
    '--yes',
    `create-react-native-library@${cfg.createLibraryVersion}`,
    '--slug',
    'warmlib',
    '--description',
    'warmlib',
    '--author-name',
    'RNWBot',
    '--author-email',
    'rnbot@example.com',
    '--author-url',
    'https://example.com',
    '--repo-url',
    'https://example.com',
    '--languages',
    cfg.languages,
    '--type',
    cfg.type,
    ...cfg.tools.map(t => `--tools=${t}`),
    '--react-native-version',
    versions.reactNative,
    '--example',
    cfg.example,
    'warmlib',
  ];
  try {
    runTool(
      'npx',
      args,
      workDir,
      {
        npm_config_userconfig: mctx.npmrcPath,
        npm_config_registry: mctx.npmRegistryUrl,
      },
      `scaffold cRNL ${branch.name} (react-native ${versions.reactNative})`,
    );
  } catch (err) {
    // npx failed before we returned workDir, so the caller's cleanup never runs;
    // remove the temp tree here to avoid leaking warm-crnl-* on every failed run.
    rmSync(workDir, {recursive: true, force: true});
    throw err;
  }
  return {workDir, projectDir: join(workDir, 'warmlib')};
}

export const createReactNativeLibraryModule: SpecialModule = {
  name: 'create-react-native-library',
  async collectDepSpecs(mctx, config) {
    const cfg = parseCrnlConfig(config);
    const sets: DepSpecSet[] = [];
    const failures: string[] = [];
    for (const branch of cfg.branches) {
      mctx.ctx.log.info(`cRNL: preparing closure for branch ${branch.name}`);
      try {
        const versions = await resolveBranchVersions(mctx, branch);
        const {workDir, projectDir} = scaffold(mctx, cfg, branch, versions);
        try {
          sets.push({
            label: `crnl:${branch.name}`,
            specs: collectScaffoldSpecs(projectDir, versions),
          });
        } finally {
          rmSync(workDir, {recursive: true, force: true});
        }
      } catch (err) {
        // One branch failing must not drop the others; record it so the caller can
        // still surface a non-zero exit while the healthy branches are warmed.
        mctx.ctx.log.error(
          `cRNL: branch ${branch.name} failed: ${(err as Error).message}`,
        );
        failures.push(branch.name);
      }
    }
    return {sets, failures};
  },
};
