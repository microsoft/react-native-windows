/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from 'graceful-fs';
import util from 'util';

const importedMethods = [
  'access',
  'appendFile',
  'chmod',
  'chown',
  'copyFile',
  'lchmod',
  'lchown',
  'lutimes',
  'link',
  'lstat',
  'mkdir',
  'mkdtemp',
  'open',
  'opendir',
  'readdir',
  'readFile',
  'readlink',
  'realpath',
  'rename',
  'rmdir',
  'rm',
  'stat',
  'symlink',
  'truncate',
  'unlink',
  'utimes',
  'writeFile',
] as const;

type ValueOf<T extends readonly any[]> = T[number];
type ImportedMethodNames = ValueOf<typeof importedMethods>;
type ImportedMethods = Pick<typeof fs, ImportedMethodNames>;

type PromisifedMethods = {
  access: typeof fs.access.__promisify__;
  appendFile: typeof fs.appendFile.__promisify__;
  chmod: typeof fs.chmod.__promisify__;
  chown: typeof fs.chown.__promisify__;
  copyFile: typeof fs.copyFile.__promisify__;
  lchmod: typeof fs.lchmod.__promisify__;
  lchown: typeof fs.lchown.__promisify__;
  lutimes: typeof fs.lutimes.__promisify__;
  link: typeof fs.link.__promisify__;
  lstat: typeof fs.lstat.__promisify__;
  mkdir: typeof fs.mkdir.__promisify__;
  mkdtemp: typeof fs.mkdtemp.__promisify__;
  open: typeof fs.open.__promisify__;
  opendir: typeof fs.opendir.__promisify__;
  readdir: typeof fs.readdir.__promisify__;
  readFile: typeof fs.readFile.__promisify__;
  readlink: typeof fs.readlink.__promisify__;
  realpath: typeof fs.realpath.__promisify__;
  rename: typeof fs.rename.__promisify__;
  rmdir: typeof fs.rmdir.__promisify__;
  rm: typeof fs.rm.__promisify__;
  stat: typeof fs.stat.__promisify__;
  symlink: typeof fs.symlink.__promisify__;
  truncate: typeof fs.truncate.__promisify__;
  unlink: typeof fs.unlink.__promisify__;
  utimes: typeof fs.utimes.__promisify__;
  writeFile: typeof fs.writeFile.__promisify__;
};

type ExtraMethods = {
  exists: (path: fs.PathLike) => Promise<boolean>;
  readJsonFile: <T = Record<string, unknown>>(
    path: fs.PathLike | number,
    options?: {encoding?: null | BufferEncoding; flag?: string | undefined},
  ) => Promise<T>;
};

export type AsyncMethods = PromisifedMethods & ExtraMethods;

const asyncMethods: AsyncMethods = {
  ...promisifyImportedMethods(),

  exists: (path: fs.PathLike) =>
    new Promise(resolve => {
      fs.access(path, fs.constants.F_OK, err =>
        err ? resolve(false) : resolve(true),
      );
    }),

  readJsonFile: async (
    path: fs.PathLike | number,
    options?: {encoding?: null | BufferEncoding; flag?: string | undefined},
  ) => {
    const opts = {
      encoding: options?.encoding || 'utf-8',
      flag: options?.flag,
    };
    return JSON.parse(await asyncMethods.readFile(path, opts));
  },
};

export default asyncMethods;

/**
 * Import the patched callbacks from `graceful-fs` to promisify ourselves, since
 * `graceful-fs` does not patch `fs.promises`.
 */
function importMethods(): ImportedMethods {
  const copiedMethods: Partial<ImportedMethods> = {};
  for (const methodName of importedMethods) {
    // @ts-ignore
    copiedMethods[methodName] = fs[methodName];
  }
  return copiedMethods as ImportedMethods;
}

function promisifyImportedMethods(): PromisifedMethods {
  const promisifiedMethods: Partial<PromisifedMethods> = {};
  for (const [methodName, method] of Object.entries(importMethods())) {
    // @ts-ignore
    promisifiedMethods[methodName] = util.promisify(method);
  }

  return promisifiedMethods as PromisifedMethods;
}
