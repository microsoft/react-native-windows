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
  'lutimesSync',
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
  'readvSync',
  'realpathSync',
  'realpathSync',
  'renameSync',
  'rmdirSync',
  'rmSync',
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
