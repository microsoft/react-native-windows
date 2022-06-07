/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from 'graceful-fs';

const importedMethods = [
  'accessSync',
  'appendFileSync',
  'chmodSync',
  'chownSync',
  'closeSync',
  'copyFileSync',
  'createReadStream',
  'createWriteStream',
  'existsSync',
  'fchmodSync',
  'fchownSync',
  'fdatasyncSync',
  'fstatSync',
  'fsyncSync',
  'ftruncateSync',
  'futimesSync',
  'lchmodSync',
  'lchownSync',
  // Not available in Node 12
  // 'lutimesSync',
  'linkSync',
  'lstatSync',
  'mkdirSync',
  'mkdtempSync',
  'opendirSync',
  'openSync',
  'readdirSync',
  'readlinkSync',
  'readFileSync',
  'readSync',
  // Not available in Node 12
  // 'readvSync',
  'realpathSync',
  'realpathSync',
  'renameSync',
  'rmdirSync',
  // Not available in Node 12
  // 'rmSync',
  'statSync',
  'symlinkSync',
  'truncateSync',
  'unlinkSync',
  'utimesSync',
  'watch',
  'watchFile',
  'writeFileSync',
  'writeSync',
  'writeSync',
  'writevSync',
] as const;

type ValueOf<T extends readonly any[]> = T[number];
type ImportedMethodNames = ValueOf<typeof importedMethods>;
type ImportedMethods = Pick<typeof fs, ImportedMethodNames>;

type ExtraMethods = {
  /**
   * Synchronously reads the contents of a file, then parses it as JSON.
   *
   * By default the parsed JSON file is assumed to be a JSON object, with no
   * assumptions made about content (`Record<string, unknown`). If the type is
   * known at call-time, it can be passed to this function to alter the return
   * type.This type is not validated to be correct at runtime.
   *
   * E.g. `fs.readJsonFileSync<Document>(documentPath)`;
   *
   * @param path A path to a file.
   * @param options Optional set of options
   * @param options.encoding The encoding of the JSON file (defaults to utf-8)
   * @param options.flag File access flags
   */
  readJsonFileSync: <T = Record<string, unknown>>(
    path: fs.PathLike | number,
    options?: {encoding?: null | BufferEncoding; flag?: string | undefined},
  ) => T;
};

export type SyncMethods = ImportedMethods & ExtraMethods;

const syncMethods: SyncMethods = {
  ...importMethods(),

  readJsonFileSync: (
    path: fs.PathLike | number,
    options?: {encoding?: null | BufferEncoding; flag?: string | undefined},
  ) => {
    const opts = {
      encoding: options?.encoding || 'utf-8',
      flag: options?.flag,
    };
    return JSON.parse(syncMethods.readFileSync(path, opts));
  },
};

export default syncMethods;

function importMethods(): ImportedMethods {
  const copiedMethods: Partial<ImportedMethods> = {};
  for (const methodName of importedMethods) {
    // @ts-ignore
    copiedMethods[methodName] = fs[methodName];
  }
  return copiedMethods as ImportedMethods;
}
