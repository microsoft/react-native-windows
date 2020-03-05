/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as FileRepository from '../FileRepository';
import * as ManifestData from '../ManifestData';

import * as ora from 'ora';
import * as path from 'path';

import GitReactFileRepository from '../GitReactFileRepository';
import Manifest from '../Manifest';
import OverrideFileRepositoryImpl from '../OverrideFileRepositoryImpl';

import {diff_match_patch} from 'diff-match-patch';
import {getInstalledRNVersion} from '../ReactVersion';

const WIN_PLATFORM_EXT = /\.win32|\.windows|\.windesktop/;
const WHITESPACE_PATTERN = /\s/g;

(async () => {
  const ovrPath = process.argv[2];
  if (!ovrPath) {
    throw new Error('Expected ovrPath to be provided');
  }

  const spinner = ora();
  spinner.start('Creating manifest');

  const version = await getInstalledRNVersion(ovrPath);
  const [overrides, reactSources] = await getFileRepos(ovrPath, version);
  const manifest: ManifestData.Manifest = {overrides: []};
  const overrideFiles = await overrides.listFiles();

  let i = 0;
  for (const file of overrideFiles) {
    spinner.text = `Creating manifest (${++i}/${overrideFiles.length})`;

    const contents = await overrides.getFileContents(file);
    (await tryAddCopy(file, version, contents, reactSources, manifest)) ||
      (await tryAddPatch(file, version, contents, reactSources, manifest)) ||
      (await tryAddDerived(file, version, contents, reactSources, manifest)) ||
      addUnknown(file, version, manifest);
  }

  const ovrFile = path.join(ovrPath, 'overrides.json');
  await ManifestData.writeToFile(manifest, ovrFile);

  spinner.succeed();
})();

async function tryAddCopy(
  filename: string,
  rnVersion: string,
  override: string,
  reactSources: FileRepository.ReactFileRepository,
  manifest: ManifestData.Manifest,
): Promise<boolean> {
  const baseContents = await reactSources.getFileContents(filename);

  const baseSignificantChars = baseContents.replace(WHITESPACE_PATTERN, '');
  const ovrSignificantChars = override.replace(WHITESPACE_PATTERN, '');
  if (baseSignificantChars !== ovrSignificantChars) {
    return false;
  }

  manifest.overrides.push({
    type: 'copy',
    file: filename,
    baseFile: filename,
    baseVersion: rnVersion,
    baseHash: Manifest.hashContent(baseContents),
    issue: 0,
  });

  return true;
}

async function tryAddPatch(
  filename: string,
  rnVersion: string,
  override: string,
  reactSources: FileRepository.ReactFileRepository,
  manifest: ManifestData.Manifest,
): Promise<boolean> {
  const baseFile = filename.replace(WIN_PLATFORM_EXT, '');
  const baseContents = await reactSources.getFileContents(baseFile);

  if (!baseContents) {
    return false;
  }

  const {similar} = computeSimilarity(override, baseContents);
  if (similar) {
    manifest.overrides.push({
      type: 'patch',
      file: filename,
      baseFile: baseFile,
      baseVersion: rnVersion,
      baseHash: Manifest.hashContent(baseContents),
      issue: 'LEGACY_FIXME',
    });
  } else {
    addUnknown(filename, rnVersion, manifest);
  }

  return true;
}

async function tryAddDerived(
  filename: string,
  rnVersion: string,
  override: string,
  reactSources: FileRepository.ReactFileRepository,
  manifest: ManifestData.Manifest,
): Promise<boolean> {
  const matches: Array<{file: string; contents: string; dist: number}> = [];

  const droidFile = filename.replace(WIN_PLATFORM_EXT, '.android');
  const droidContents = await reactSources.getFileContents(droidFile);
  const droidSim = droidContents && computeSimilarity(override, droidContents);
  if (droidSim && droidSim.similar) {
    matches.push({
      file: droidFile,
      contents: droidContents,
      dist: droidSim.editDistance,
    });
  }

  const iosFile = filename.replace(WIN_PLATFORM_EXT, '.ios');
  const iosContents = await reactSources.getFileContents(iosFile);
  const iosSim = iosContents && computeSimilarity(override, iosContents);
  if (iosSim && iosSim.similar) {
    matches.push({
      file: iosFile,
      contents: iosContents,
      dist: iosSim.editDistance,
    });
  }

  if (matches.length === 0) {
    return false;
  }

  const bestMatch = matches.reduce((a, b) => (a.dist < b.dist ? a : b));
  manifest.overrides.push({
    type: 'derived',
    file: filename,
    baseFile: bestMatch.file,
    baseVersion: rnVersion,
    baseHash: Manifest.hashContent(bestMatch.contents),
    issue: 'LEGACY_FIXME',
  });

  return true;
}

function addUnknown(
  filename: string,
  rnVersion: string,
  manifest: ManifestData.Manifest,
) {
  (manifest.overrides as Array<any>).push({
    type: '???',
    file: filename,
    baseFile: '???',
    baseVersion: rnVersion,
    baseHash: '???',
    issue: 'LEGACY_FIXME',
  });
}

async function getFileRepos(
  overrideovrPath: string,
  rnVersion: string,
): Promise<
  [FileRepository.OverrideFileRepository, FileRepository.ReactFileRepository]
> {
  const ovrPattern = /^.*\.(js|ts|jsx|tsx)$/;
  const overrides = new OverrideFileRepositoryImpl(overrideovrPath, ovrPattern);

  const versionedReactSources = await GitReactFileRepository.createAndInit();
  const reactSources = FileRepository.bindVersion(
    versionedReactSources,
    rnVersion,
  );

  return [overrides, reactSources];
}

function computeSimilarity(
  override: string,
  source: string,
): {similar: boolean; editDistance: number} {
  override = stripCopyrightHeaders(override);
  source = stripCopyrightHeaders(source);

  const differ = new diff_match_patch();
  const diffs = differ.diff_main(source, override);

  const editDistance = differ.diff_levenshtein(diffs);
  const similar = editDistance / Math.max(override.length, source.length) < 0.8;
  return {similar, editDistance};
}

function stripCopyrightHeaders(str: string): string {
  if (!str.startsWith('/*')) {
    return str;
  }

  const headerEnd = str.indexOf('*/') + 1;
  return str.slice(headerEnd);
}
