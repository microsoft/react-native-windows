/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as FileRepository from '../FileRepository';
import * as Serialized from '../Serialized';

import * as ora from 'ora';
import * as path from 'path';
import * as yargs from 'yargs';

import FileSystemRepository from '../FileSystemRepository';
import GitReactFileRepository from '../GitReactFileRepository';

import {diff_match_patch} from 'diff-match-patch';
import {getInstalledRNVersion} from '../PackageUtils';
import {hashContent} from '../Hash';

import isUtf8 from 'isutf8';

const {extensions, directory} = yargs
  .options({
    directory: {
      description:
        'The directory containing files that should be in the manifest',
      type: 'string',
      demandOption: true,
    },
    extensions: {
      description: 'A list of platform specific JS extensions to include',
      type: 'array',
      demandOption: true,
    },
  })
  .version(false).argv;

const extensionsRegex = new RegExp(extensions.map(e => `\\.${e}`).join('|'));

(async () => {
  const spinner = ora();
  spinner.start('Creating manifest');

  const version = await getInstalledRNVersion();
  const [overrides, reactSources] = await getFileRepos(directory, version);
  const manifest: Serialized.Manifest = {
    includePatterns: undefined,
    excludePatterns: undefined,
    baseVersion: version,
    overrides: [],
  };
  const overrideFiles = await overrides.listFiles();

  let i = 0;
  for (const file of overrideFiles) {
    spinner.text = `Creating manifest (${++i}/${overrideFiles.length})`;

    const contents = (await overrides.readFile(file))!;
    (await tryAddCopy(file, contents, reactSources, manifest)) ||
      (await tryAddPatch(file, contents, reactSources, manifest)) ||
      (await tryAddDerived(file, contents, reactSources, manifest)) ||
      addUnknown(file, manifest);
  }

  const ovrFile = path.join(directory, 'overrides.json');
  await Serialized.writeManifestToFile(manifest, ovrFile);

  spinner.succeed();
})();

async function tryAddCopy(
  filename: string,
  overrideContent: Buffer,
  reactSources: FileRepository.ReactFileRepository,
  manifest: Serialized.Manifest,
): Promise<boolean> {
  const baseContent = await reactSources.readFile(filename);
  if (!baseContent) {
    return false;
  }

  if (hashContent(baseContent) !== hashContent(overrideContent)) {
    return false;
  }

  manifest.overrides.push({
    type: 'copy',
    file: filename,
    baseFile: filename,
    baseVersion: undefined,
    baseHash: hashContent(baseContent),
    issue: undefined,
  });

  return true;
}

async function tryAddPatch(
  filename: string,
  overrideContent: Buffer,
  reactSources: FileRepository.ReactFileRepository,
  manifest: Serialized.Manifest,
): Promise<boolean> {
  const baseFile = filename.replace(extensionsRegex, '');
  const baseContent = await reactSources.readFile(baseFile);

  if (!baseContent) {
    return false;
  }

  const {similar} = computeSimilarity(overrideContent, baseContent);
  if (similar) {
    manifest.overrides.push({
      type: 'patch',
      file: filename,
      baseFile: baseFile,
      baseVersion: undefined,
      baseHash: hashContent(baseContent),
      issue: undefined,
    });
  } else {
    addUnknown(filename, manifest);
  }

  return true;
}

async function tryAddDerived(
  filename: string,
  overrideContent: Buffer,
  reactSources: FileRepository.ReactFileRepository,
  manifest: Serialized.Manifest,
): Promise<boolean> {
  const matches: Array<{file: string; contents: Buffer; dist: number}> = [];

  const droidFile = filename.replace(extensionsRegex, '.android');
  const droidContents = await reactSources.readFile(droidFile);
  const droidSim =
    droidContents && computeSimilarity(overrideContent, droidContents);
  if (droidSim && droidSim.similar) {
    matches.push({
      file: droidFile,
      contents: droidContents!,
      dist: droidSim.editDistance,
    });
  }

  const iosFile = filename.replace(extensionsRegex, '.ios');
  const iosContents = await reactSources.readFile(iosFile);
  const iosSim = iosContents && computeSimilarity(overrideContent, iosContents);
  if (iosSim && iosSim.similar) {
    matches.push({
      file: iosFile,
      contents: iosContents!,
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
    baseVersion: undefined,
    baseHash: hashContent(bestMatch.contents),
    issue: undefined,
  });

  return true;
}

function addUnknown(filename: string, manifest: Serialized.Manifest) {
  (manifest.overrides as Array<any>).push({
    type: '???',
    file: filename,
    baseFile: '???',
    baseHash: '???',
  });
}

async function getFileRepos(
  overrideovrPath: string,
  rnVersion: string,
): Promise<
  [FileRepository.WritableFileRepository, FileRepository.ReactFileRepository]
> {
  const overrides = new FileSystemRepository(overrideovrPath);

  const versionedReactSources = await GitReactFileRepository.createAndInit();
  const reactSources = FileRepository.bindVersion(
    versionedReactSources,
    rnVersion,
  );

  return [overrides, reactSources];
}

function computeSimilarity(
  override: Buffer,
  source: Buffer,
): {similar: boolean; editDistance: number} {
  if (!isUtf8(override) || !isUtf8(source)) {
    return {similar: false, editDistance: NaN};
  }

  let overrideStr = override.toString();
  let sourceStr = source.toString();

  overrideStr = stripCopyrightHeaders(overrideStr);
  sourceStr = stripCopyrightHeaders(sourceStr);

  const differ = new diff_match_patch();
  const diffs = differ.diff_main(sourceStr, overrideStr);

  const editDistance = differ.diff_levenshtein(diffs);
  const similar =
    editDistance / Math.max(overrideStr.length, sourceStr.length) < 0.8;
  return {similar, editDistance};
}

function stripCopyrightHeaders(str: string): string {
  if (!str.startsWith('/*')) {
    return str;
  }

  const headerEnd = str.indexOf('*/') + 1;
  return str.slice(headerEnd);
}
