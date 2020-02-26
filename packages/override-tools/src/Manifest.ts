/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import * as crypto from 'crypto';
import * as fs from 'fs';
import * as path from 'path';
import * as t from 'io-ts';

import {OverrideFileRepository, ReactFileRepository} from './FileRepository';

import {ThrowReporter} from 'io-ts/lib/ThrowReporter';

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

  // Allow LEGACY_FIXME for existing overrides that don't have issues yet
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
  const json = (await fs.promises.readFile(filePath)).toString();
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

  // Make sure directory separators go the right direction
  (parsed as Manifest).overrides.forEach(override => {
    override.file = path.normalize(override.file);
    if (override.type !== 'platform') {
      override.baseFile = path.normalize(override.baseFile);
    }
  });

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
  overrideRepo: OverrideFileRepository,
  reactRepo: ReactFileRepository,
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
