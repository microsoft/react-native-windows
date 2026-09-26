/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * create-react-native-app special module.
 *
 * The CLI-init *app* test doesn't install react-native-windows' own dependencies —
 * it scaffolds a standalone app with `@react-native-community/cli init` (pulling
 * `@react-native-community/template`) and installs *that generated app*. So the
 * closure the feed needs is the generated app's dependency graph (e.g.
 * `react-native-safe-area-context`), which enumeration can't discover. This module
 * reproduces that scaffold and resolves the generated app's closure. It is the app
 * counterpart to the create-react-native-library module (the lib test), and mirrors
 * `vnext/Scripts/creaternwapp.cmd`.
 *
 * warm-feed runs only from `main`, but the test runs on every release branch, each
 * pinned to a different React Native. The config manifest therefore lists all
 * branches; per branch we derive the RN/CLI versions (shared with the lib module:
 * nightly for `main` from the working-tree vnext/package.json; for a stable branch,
 * the react-native it pins in its own vnext/package.json, read via git), scaffold,
 * and read the generated app manifest.
 *
 * Template selection mirrors creaternwapp.cmd exactly: a stable branch lets the CLI
 * pick the template that matches its React Native, while a nightly RN overrides the
 * template to a fixed stable one (`@react-native-community/template@0.86.0`) so its
 * inner CLI never requests a `nightly` template. Keep this in sync with the
 * `#15124`-tagged override in creaternwapp.cmd.
 *
 * @format
 */

import {mkdtempSync, readFileSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {runTool, type DepSpecSet} from '../closure';
import {manifestSpecs} from '../manifest';
// Branch config and version resolution are shared with the lib module, which owns
// them today; the app module reuses them rather than re-deriving RN/CLI/RNW versions.
import {
  nightlyFixupSpecs,
  readRnwWorkspaceSpecs,
  resolveBranchVersions,
  type BranchVersions,
  type CrnlBranch,
} from './createReactNativeLibrary';
import type {SpecialModule, SpecialModuleContext} from './types';

/** The stable template creaternwapp.cmd pins when React Native is a nightly (#15124). */
const DEFAULT_NIGHTLY_TEMPLATE = '@react-native-community/template@0.86.0';

/** A space-free app name so the Windows shell launch needs no quoting. */
const DEFAULT_APP_NAME = 'warmapp';

interface CrnAppConfig {
  /** `@react-native-community/template@<version>` used when the RN is a nightly. */
  nightlyTemplate: string;
  /** Name of the scaffolded app directory. */
  appName: string;
  branches: CrnlBranch[];
}

function parseBranch(raw: unknown): CrnlBranch {
  const rec = (raw ?? {}) as Record<string, unknown>;
  if (typeof rec.name !== 'string' || !rec.name) {
    throw new Error("each cRNApp branch requires a 'name'");
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
    ref: typeof rec.ref === 'string' ? rec.ref : undefined,
  };
}

export function parseCrnAppConfig(
  config: Record<string, unknown>,
): CrnAppConfig {
  const branchesRaw = config.branches;
  if (!Array.isArray(branchesRaw) || branchesRaw.length === 0) {
    throw new Error(
      "create-react-native-app module requires a non-empty 'branches' list",
    );
  }
  return {
    nightlyTemplate:
      typeof config.nightlyTemplate === 'string' && config.nightlyTemplate
        ? config.nightlyTemplate
        : DEFAULT_NIGHTLY_TEMPLATE,
    appName:
      typeof config.appName === 'string' && config.appName
        ? config.appName
        : DEFAULT_APP_NAME,
    branches: branchesRaw.map(parseBranch),
  };
}

/**
 * The `--template` args passed to `cli init`, mirroring creaternwapp.cmd: none for a
 * stable RN (the CLI's default template matches the version), the fixed stable
 * template for a nightly RN (so the inner CLI never requests a nightly template).
 */
export function templateArgs(cfg: CrnAppConfig, nightly: boolean): string[] {
  return nightly ? ['--template', cfg.nightlyTemplate] : [];
}

/**
 * The generated app manifest as a single spec set. For a nightly branch the
 * generated RN-family/CLI specs are rewritten to the nightly versions (mirroring
 * vnext/Scripts/UpdateNightlyDependencies.ps1, as the lib module does), and the
 * resolved react-native-windows spec is added (the test installs it too).
 */
export function appSpecsFromManifest(
  manifest: unknown,
  versions: BranchVersions,
  branchName: string,
): DepSpecSet[] {
  const specs = manifestSpecs(manifest);
  if (versions.nightly) {
    nightlyFixupSpecs(specs, versions.reactNative, versions.reactNativeCli);
  }
  if (versions.reactNativeWindowsSpec) {
    specs['react-native-windows'] = versions.reactNativeWindowsSpec;
  }
  return [{label: `crnapp:${branchName}:app`, specs}];
}

function scaffold(
  mctx: SpecialModuleContext,
  cfg: CrnAppConfig,
  branch: CrnlBranch,
  versions: BranchVersions,
): {workDir: string; projectDir: string} {
  if (!versions.reactNativeCli) {
    throw new Error(
      `cRNApp: could not determine a @react-native-community/cli version for branch '${branch.name}' (set reactNativeCliVersion)`,
    );
  }
  const workDir = mkdtempSync(join(tmpdir(), 'warm-crnapp-'));
  const args = [
    '--yes',
    `@react-native-community/cli@${versions.reactNativeCli}`,
    'init',
    cfg.appName,
    '--version',
    versions.reactNative,
    ...templateArgs(cfg, versions.nightly),
    // Match creaternwapp.cmd: resolve metadata only, no install/pods/git in the warmer.
    '--skip-install',
    '--install-pods',
    'false',
    '--skip-git-init',
    'true',
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
      `scaffold cRNApp ${branch.name} (react-native ${versions.reactNative})`,
    );
  } catch (err) {
    // npx failed before we returned workDir, so the caller's cleanup never runs;
    // remove the temp tree here to avoid leaking warm-crnapp-* on every failed run.
    rmSync(workDir, {recursive: true, force: true});
    throw err;
  }
  return {workDir, projectDir: join(workDir, cfg.appName)};
}

export const createReactNativeAppModule: SpecialModule = {
  name: 'create-react-native-app',
  async collectDepSpecs(mctx, config) {
    const cfg = parseCrnAppConfig(config);
    const sets: DepSpecSet[] = [];
    const failures: string[] = [];
    for (const branch of cfg.branches) {
      mctx.ctx.log.info(`cRNApp: preparing closure for branch ${branch.name}`);
      try {
        const versions = await resolveBranchVersions(mctx, branch);
        const {workDir, projectDir} = scaffold(mctx, cfg, branch, versions);
        try {
          const manifest = JSON.parse(
            readFileSync(join(projectDir, 'package.json'), 'utf8'),
          );
          sets.push(...appSpecsFromManifest(manifest, versions, branch.name));
        } finally {
          rmSync(workDir, {recursive: true, force: true});
        }
        if (versions.nightly) {
          // The main app-init job installs the locally built RNW, whose manifest
          // pins exact canary @react-native-windows/* versions the feed must have.
          const rnwSpecs = await readRnwWorkspaceSpecs(mctx);
          if (Object.keys(rnwSpecs).length > 0) {
            sets.push({
              label: `crnapp:${branch.name}:rnw-workspace`,
              specs: rnwSpecs,
            });
          }
        }
      } catch (err) {
        // One branch failing must not drop the others; record it so the caller can
        // still surface a non-zero exit while the healthy branches are warmed.
        mctx.ctx.log.error(
          `cRNApp: branch ${branch.name} failed: ${(err as Error).message}`,
        );
        failures.push(branch.name);
      }
    }
    return {sets, failures};
  },
};
