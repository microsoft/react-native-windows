/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as Serialized from './Serialized';
import * as fs from 'fs';
import * as path from 'path';

import OverrideFactory, {OverrideFactoryImpl} from './OverrideFactory';
import {
  ReactFileRepository,
  WritableFileRepository,
  bindVersion,
} from './FileRepository';
import FileSystemRepository from './FileSystemRepository';
import GitReactFileRepository from './GitReactFileRepository';
import Manifest from './Manifest';
import Override from './Override';
import {UpgradeResult} from './UpgradeStrategy';
import {ValidationError} from './ValidationStrategy';
import {findManifest} from './FileSearch';
import {getInstalledRNVersion} from './PackageUtils';

// Re-export types used in the public API so external packages don't have to
// reach into our guts to import them.
export * from './OverrideFactory';
export * from './Override';
export {UpgradeResult, ValidationError};

/**
 * Check that the given manifest correctly describe overrides and that all
 * overrides are up to date
 */
export async function validateManifest(opts: {
  manifestPath?: string;
  reactNativeVersion?: string;
}): Promise<ValidationError[]> {
  const {manifest, overrideRepo, reactRepo} = await createManifestContext(opts);
  return await manifest.validate(overrideRepo, reactRepo);
}

/**
 * Return whether the override exists in the manifest
 */
export async function hasOverride(
  overrideName: string,
  opts: {manifestPath?: string},
): Promise<boolean> {
  const {manifest} = await createManifestContext(opts);
  return manifest.hasOverride(overrideName);
}

/**
 * Removes an override from the manifest if it exists.
 * @returns whether the override was removed
 */
export async function removeOverride(
  overrideName: string,
  opts: {manifestPath?: string},
): Promise<boolean> {
  const {manifest, manifestPath} = await createManifestContext(opts);
  const removed = manifest.removeOverride(overrideName);
  if (removed) {
    await Serialized.writeManifestToFile(manifest.serialize(), manifestPath);
  }

  return removed;
}

/**
 * Returns a factory to create overrides which may be added to the manifest
 */
export async function getOverrideFactory(opts: {
  manifestPath?: string;
  reactNativeVersion?: string;
}): Promise<OverrideFactory> {
  return (await createManifestContext(opts)).overrideFactory;
}

/**
 * Adds an override to the manifest
 */
export async function addOverride(
  override: Override,
  opts: {manifestPath?: string},
): Promise<void> {
  const {manifest, manifestPath} = await createManifestContext(opts);
  await manifest.addOverride(override);
  await Serialized.writeManifestToFile(manifest.serialize(), manifestPath);
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
export async function upgradeOverrides(opts: {
  manifestPath?: string;
  reactNativeVersion?: string;
  allowConflicts: boolean;
  newVersion?: string;
  progressListener?: UpgradeProgressListener;
}): Promise<UpgradeResult[]> {
  const ctx = await createManifestContext(opts);

  const outOfDateOverrides = (await ctx.manifest.validate(
    ctx.overrideRepo,
    ctx.reactRepo,
  ))
    .filter(err => err.type === 'outOfDate')
    .map(err => err.overrideName)
    .map(ovrName => ctx.manifest.findOverride(ovrName)!);

  const upgradeResults: Array<UpgradeResult> = [];

  let i = 0;
  await Promise.all(
    outOfDateOverrides.map(async override => {
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

      upgradeResults.push(upgradeResult);

      if (upgradeResult.filesWritten) {
        await ctx.manifest.markUpToDate(override.name(), ctx.overrideFactory);
      }
    }),
  );

  if (upgradeResults.length > 0) {
    await Serialized.writeManifestToFile(
      ctx.manifest.serialize(),
      ctx.manifestPath,
    );
  }

  return upgradeResults;
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
  manifestPath: string;
  reactNativeVersion: string;
}

/**
 * Sets up state for a manifest describing overrides at a specified RN version
 */
async function createManifestContext(opts: {
  manifestPath?: string;
  reactNativeVersion?: string;
}): Promise<ManifestContext> {
  const manifestPath = opts.manifestPath || (await findManifest());

  await checkFileExists('manifest', manifestPath);
  const reactNativeVersion =
    opts.reactNativeVersion || (await getInstalledRNVersion());

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
    manifestPath,
    manifest,
    reactNativeVersion,
  };
}
