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
  'readFileSync',
  'readlinkSync',
  'readSync',
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
type ImportedMethods = ValueOf<typeof importedMethods>;

export type SyncMethods = Omit<
  Pick<typeof fs, ImportedMethods>,
  'readFileSync'
> & {
  readFileSync: typeof fs.readFileSync & ReadFileAsMethods;
};

type ReadFileAsMethods = {
  asJson: <T = Record<string, unknown> | unknown[]>(
    path: fs.PathLike | number,
    options?: {encoding?: null | BufferEncoding; flag?: string | undefined},
  ) => T;
};

const copiedMethods: Partial<Pick<typeof fs, ImportedMethods>> = {};
for (const methodName of importedMethods) {
  // @ts-ignore
  copiedMethods[methodName] = fs[methodName];
}

const syncMethods: SyncMethods = {
  ...(copiedMethods as Omit<SyncMethods, 'readFileSync'>),

  readFileSync: Object.assign(fs.readFileSync, {
    asJson: (
      path: fs.PathLike | number,
      options?: {encoding?: null | BufferEncoding; flag?: string | undefined},
    ) => {
      const opts = {
        encoding: options?.encoding || 'utf-8',
        flag: options?.flag,
      };
      return JSON.parse(syncMethods.readFileSync(path, opts));
    },
  }),
};

export default syncMethods;
