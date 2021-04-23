/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// Typings for "async" confuse this rule
/* eslint-disable @typescript-eslint/no-misused-promises */

import * as Serialized from './Serialized';
import * as _ from 'lodash';
import * as fs from 'fs';
import * as path from 'path';

import OverrideFactory, {OverrideFactoryImpl} from './OverrideFactory';
import {
  ReactFileRepository,
  WritableFileRepository,
  bindVersion,
} from './FileRepository';
import {eachLimit, mapLimit} from 'async';
import FileSystemRepository from './FileSystemRepository';
import GitReactFileRepository from './GitReactFileRepository';
import Manifest from './Manifest';
import Override from './Override';
import {UpgradeResult} from './UpgradeStrategy';
import {ValidationError} from './ValidationStrategy';
import {getInstalledRNVersion} from './PackageUtils';

// Re-export types used in the public API so external packages don't have to
// reach into our guts to import them.
export * from './OverrideFactory';
export * from './Override';
export {UpgradeResult, ValidationError};

const MAX_CONCURRENT_TASKS = 30;

/**
 * Check that the given manifest correctly describe overrides and that all
 * overrides are up to date
 */
export async function validateManifest(
  manifestPath: string,
  opts?: {
    reactNativeVersion?: string;
  },
): Promise<ValidationError[]> {
  const {manifest, overrideRepo, reactRepo} = await createManifestContext(
    manifestPath,
    opts,
  );
  return await manifest.validate(overrideRepo, reactRepo);
}

/**
 * Return whether the override exists in the manifest
 */
export async function hasOverride(
  overrideName: string,
  manifestPath: string,
): Promise<boolean> {
  const {manifest} = await createManifestContext(manifestPath);
  return manifest.hasOverride(overrideName);
}

/**
 * Removes an override from the manifest if it exists.
 * @returns whether the override was removed
 */
export async function removeOverride(
  overrideName: string,
  manifestPath: string,
): Promise<boolean> {
  const {manifest} = await createManifestContext(manifestPath);
  const removed = manifest.removeOverride(overrideName);
  if (removed) {
    await Serialized.writeManifestToFile(manifest.serialize(), manifestPath);
  }

  return removed;
}

/**
 * Returns a factory to create overrides which may be added to the manifest
 */
export async function getOverrideFactory(
  manifestPath: string,
  opts?: {
    reactNativeVersion?: string;
  },
): Promise<OverrideFactory> {
  return (await createManifestContext(manifestPath, opts)).overrideFactory;
}

/**
 * Adds an override to the manifest
 */
export async function addOverride(
  override: Override,
  manifestPath: string,
): Promise<void> {
  const {manifest} = await createManifestContext(manifestPath);
  manifest.addOverride(override);
  await Serialized.writeManifestToFile(manifest.serialize(), manifestPath);
}

/**
 * Ouputs a patch-style diff of an override compared to its original source
 */
export async function diffOverride(
  overrideName: string,
  manifestPath: string,
): Promise<string> {
  const ctx = await createManifestContext(manifestPath);

  const override = ctx.manifest.findOverride(overrideName);
  if (!override) {
    throw new Error(`Could not find override with name "${overrideName}"`);
  }

  return override.diffStrategy().diff(ctx.gitReactRepo, ctx.overrideRepo);
}

/**
 * Receives notifications on progress during overide upgrades
 */
export type UpgradeProgressListener = (
  currentOverride: number,
  totalOverrides: number,
) => void;

/**
 * Attempts to automatically merge changes from the current version into
 * out-of-date overrides.
 */
export async function upgradeOverrides(
  manifestPath: string,
  opts: {
    allowConflicts: boolean;
    reactNativeVersion?: string;
    progressListener?: UpgradeProgressListener;
  },
): Promise<UpgradeResult[]> {
  const ctx = await createManifestContext(manifestPath, opts);

  const validationErrors = await ctx.manifest.validate(
    ctx.overrideRepo,
    ctx.reactRepo,
  );

  const outOfDateOverrides = validationErrors
    .filter(err => err.type === 'outOfDate')
    .map(err => ctx.manifest.findOverride(err.overrideName)!);

  // Perform upgrades concurrently so we can take advantage of
  // GitReactFileRepository optimizations when multiple requests are queued at
  // once.
  let i = 0;
  const upgradeResults = await mapLimit<Override, UpgradeResult>(
    outOfDateOverrides,
    MAX_CONCURRENT_TASKS,
    async override => {
      const upgradeResult = await override
        .upgradeStrategy()
        .upgrade(
          ctx.gitReactRepo,
          ctx.overrideRepo,
          ctx.reactNativeVersion,
          opts.allowConflicts,
        );

      if (opts.progressListener) {
        opts.progressListener(++i, outOfDateOverrides.length);
      }

      return upgradeResult;
    },
  );

  // Regenerate overrides that are already up to date to update the baseVersion
  // to current. This helps to minimize the numbers of versions we have to
  // check out for future upgrades.
  const upToDateOverrides = [
    ..._.difference(
      ctx.manifest.listOverrides(),
      validationErrors.map(err => ctx.manifest.findOverride(err.overrideName)!),
    ).map(ovr => ovr.name()),

    ...upgradeResults
      .filter(res => res.filesWritten)
      .map(res => res.overrideName),
  ];

  await eachLimit(upToDateOverrides, MAX_CONCURRENT_TASKS, async name => {
    await ctx.manifest.markUpToDate(name, ctx.overrideFactory);
  });

  ctx.manifest.setBaseVersion(ctx.reactNativeVersion);
  await Serialized.writeManifestToFile(ctx.manifest.serialize(), manifestPath);

  return upgradeResults.sort((a, b) =>
    a.overrideName.localeCompare(b.overrideName, 'en'),
  );
}

/**
 * Throw if a file doesn't exist, printing an error message on the way
 */
async function checkFileExists(friendlyName: string, filePath: string) {
  try {
    await fs.promises.access(filePath);
  } catch (ex) {
    throw new Error(`Could not find ${friendlyName} at path '${filePath}'`);
  }
}

/**
 * Context describing state centered around a single manifest
 */
interface ManifestContext {
  overrideRepo: WritableFileRepository;
  reactRepo: ReactFileRepository;
  gitReactRepo: GitReactFileRepository;
  overrideFactory: OverrideFactory;
  manifest: Manifest;
  reactNativeVersion: string;
}

/**
 * Sets up state for a manifest describing overrides at a specified RN version
 */
async function createManifestContext(
  manifestPath: string,
  opts?: {
    reactNativeVersion?: string;
  },
): Promise<ManifestContext> {
  await checkFileExists('manifest', manifestPath);
  const reactNativeVersion =
    opts?.reactNativeVersion || (await getInstalledRNVersion());

  const overrideDir = path.dirname(manifestPath);
  const overrideRepo = new FileSystemRepository(overrideDir);

  const gitReactRepo = await GitReactFileRepository.createAndInit();
  const reactRepo = bindVersion(gitReactRepo, reactNativeVersion);

  const overrideFactory = new OverrideFactoryImpl(reactRepo, overrideRepo);
  const manifest = Manifest.fromSerialized(
    await Serialized.readManifestFromFile(manifestPath),
  );

  return {
    overrideRepo,
    reactRepo,
    gitReactRepo,
    overrideFactory,
    manifest,
    reactNativeVersion,
  };
}
