/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';
import * as t from 'io-ts';

import {ThrowReporter} from 'io-ts/lib/ThrowReporter';

/**
 * Serialized form of {@see PlatformOverride}
 */
const PlatformOverrideType = t.type({
  type: t.literal('platform'),
  file: t.string,
});

/**
 * Serialized form of {@see PatchOverride}
 */
const PatchOverrideType = t.type({
  type: t.literal('patch'),
  file: t.string,
  baseFile: t.string,
  baseVersion: t.union([t.undefined, t.string]),
  baseHash: t.string,
  issue: t.union([t.undefined, t.number]),
});

/**
 * Serialized form of {@see DerivedOverride}
 */
const DerivedOverrideType = t.type({
  type: t.literal('derived'),
  file: t.string,
  baseFile: t.string,
  baseVersion: t.union([t.undefined, t.string]),
  baseHash: t.string,
  issue: t.union([t.undefined, t.number]),
});

/**
 * Serialized form of {@see CopyOverride}
 */
const CopyOverrideType = t.type({
  type: t.literal('copy'),
  file: t.string,
  baseFile: t.string,
  baseVersion: t.union([t.undefined, t.string]),
  baseHash: t.string,
  issue: t.union([t.undefined, t.number]),
});

/**
 * Serialized form of {@see DirectoryCopyOverride}
 */
const DirectoryCopyOverrideType = t.type({
  type: t.literal('copy'),
  directory: t.string,
  baseDirectory: t.string,
  baseVersion: t.union([t.undefined, t.string]),
  baseHash: t.string,
  issue: t.union([t.undefined, t.number]),
});

/**
 * Union of all serialized override types
 */
const OverrideType = t.union([
  PlatformOverrideType,
  PatchOverrideType,
  DerivedOverrideType,
  CopyOverrideType,
  DirectoryCopyOverrideType,
]);

/**
 * Schema for the "overrides.json" manifest
 */
const ManifestType = t.type({
  includePatterns: t.union([t.undefined, t.array(t.string)]),
  excludePatterns: t.union([t.undefined, t.array(t.string)]),
  baseVersion: t.union([t.undefined, t.string]),
  overrides: t.array(OverrideType),
});

export type PlatformOverride = t.TypeOf<typeof PlatformOverrideType>;
export type PatchOverride = t.TypeOf<typeof PatchOverrideType>;
export type DerivedOverride = t.TypeOf<typeof DerivedOverrideType>;
export type CopyOverride = t.TypeOf<typeof CopyOverrideType>;
export type DirectoryCopyOverride = t.TypeOf<typeof DirectoryCopyOverrideType>;
export type Override = t.TypeOf<typeof OverrideType>;
export type Manifest = t.TypeOf<typeof ManifestType>;

/**
 * Read an override manifest from a file.
 *
 * @throws if the file is invalid or cannot be found
 */
export async function readManifestFromFile(
  filePath: string,
): Promise<Manifest> {
  const json = (await fs.promises.readFile(filePath)).toString();
  return parseManifest(json);
}

/**
 * Parse a string with JSON for the override manifest into one.
 *
 * @throws if the JSON doesn't describe a valid manifest
 */
export function parseManifest(json: string): Manifest {
  const parsed = JSON.parse(json);

  try {
    ThrowReporter.report(ManifestType.decode(parsed));
  } catch (ex) {
    throw new Error('Could not parse manifest. Is it valid?');
  }

  return parsed;
}

/**
 * Writes the manifest to a JSON file. Does not validate correctness of the
 * manifest.
 */
export async function writeManifestToFile(
  manifest: Manifest,
  filePath: string,
) {
  const json = JSON.stringify(manifest, null /*replacer*/, 2 /*space*/);
  await fs.promises.mkdir(path.dirname(filePath), {recursive: true});
  await fs.promises.writeFile(filePath, json);
}
