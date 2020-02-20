/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import * as crypto from 'crypto';
import * as fs from './fs-promise';
import * as t from 'io-ts';

import {
  IOverrideFileRepository,
  IReactFileRepository,
} from './FileRepositoryInterface';

import {ThrowReporter} from 'io-ts/es6/ThrowReporter';

/**
 * Manifest entry type class for "platform" overrides. I.e. overrides not
 * patching shared code, or derived from existing code.
 */
const PlatformEntryType = t.type({
  type: t.literal('platform'),
  file: t.string,
});

/**
 * Manifest entry type class for overrides that derive or patch from upstream
 * code.
 */
const NonPlatformEntryType = t.type({
  type: t.union([t.literal('patch'), t.literal('derived')]),
  file: t.string,
  baseFile: t.string,
  baseVersion: t.string,
  baseHash: t.string,

  // Alloq LEGACY_FIXME for existing overrides that don't have issues yet
  issue: t.union([t.number, t.literal('LEGACY_FIXME')]),
});

const EntryType = t.union([PlatformEntryType, NonPlatformEntryType]);
const ManifestType = t.type({overrides: t.array(EntryType)});

export type PlatformEntry = t.TypeOf<typeof PlatformEntryType>;
export type NonPlatformEntry = t.TypeOf<typeof NonPlatformEntryType>;
export type Entry = t.TypeOf<typeof EntryType>;
export type Manifest = t.TypeOf<typeof ManifestType>;

/**
 * Read an override manifest from a file.
 *
 * @throws if the file is invalid or cannot be found
 */
export async function readFromFile(filePath: string): Promise<Manifest> {
  const json = (await fs.readFile(filePath)).toString();
  return this.parse(json);
}

/**
 * Parse a string with JSON for the override manifest into one.
 *
 * @throws if the JSON doesn't describe a valid manifest
 */
export function parse(json: string): Manifest {
  const parsed = JSON.parse(json);

  ThrowReporter.report(ManifestType.decode(parsed));
  return parsed;
}

export interface ValidationError {
  type:
    | 'fileMissingFromManifest' // An override file is present with no manifest entry
    | 'overrideFileNotFound' // The manifest describes a file which does not exist
    | 'baseFileNotFound' // The base file for a manifest entry cannot be found
    | 'outOfDate'; // A base file has changed since the manifested version
  file: string;
}

/**
 * Check that overrides are accurately accounted for in the manifest. I.e. we
 * should have a 1:1 mapping between files and manifest entries, and base files
 * should be present and unchanged since entry creation.
 */
export async function validate(
  manifest: Manifest,
  overrideRepo: IOverrideFileRepository,
  reactRepo: IReactFileRepository,
): Promise<Array<ValidationError>> {
  const errors: Array<ValidationError> = [];

  const manifestedFiles = manifest.overrides.map(override => override.file);
  const overrideFiles = await overrideRepo.listFiles();

  const fileMissingFromManifest = _.difference(overrideFiles, manifestedFiles);
  fileMissingFromManifest.forEach(file =>
    errors.push({type: 'fileMissingFromManifest', file: file}),
  );
  const overridesNotFound = _.difference(manifestedFiles, overrideFiles);
  overridesNotFound.forEach(file =>
    errors.push({type: 'overrideFileNotFound', file: file}),
  );

  manifest = sortIntoVersionBatches(manifest);
  await Promise.all(
    manifest.overrides.map(async override => {
      if (override.type === 'platform') {
        return;
      }

      const baseContent = await reactRepo.getFileContents(override.baseFile);
      if (baseContent === null) {
        errors.push({type: 'baseFileNotFound', file: override.baseFile});
        return;
      }

      const baseHash = hashContent(baseContent);
      if (baseHash.toLowerCase() !== override.baseHash.toLowerCase()) {
        errors.push({type: 'outOfDate', file: override.file});
        return;
      }
    }),
  );

  return errors;
}

/**
 * Hash content into the form expected in a manifest entry. Exposed for
 * testing.
 */
export function hashContent(str: string) {
  const hasher = crypto.createHash('sha1');
  hasher.update(str);
  return hasher.digest('hex');
}

/**
 * GitReactFileRepository takes time to switch between versions. Batch versions
 * together to reduce the number of checkouts that happen.
 */
function sortIntoVersionBatches(manifest: Manifest): Manifest {
  const ret = _.cloneDeep(manifest);

  ret.overrides.sort((a: Entry, b: Entry) => {
    if (a.type === 'platform' && b.type === 'platform') {
      return 0;
    } else if (a.type === 'platform') {
      return -1;
    } else if (b.type === 'platform') {
      return 1;
    } else {
      return a.baseVersion.localeCompare(b.baseVersion);
    }
  });

  return ret;
}
