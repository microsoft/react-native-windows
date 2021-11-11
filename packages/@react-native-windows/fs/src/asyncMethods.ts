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
type ImportedMethods = ValueOf<typeof importedMethods>;

export type AsyncMethods = {
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

  exists: (path: fs.PathLike) => Promise<boolean>;
  readFile: typeof fs.readFile.__promisify__ & ReadFileAsMethods;
};

type ReadFileAsMethods = {
  asJson: <T = Record<string, unknown> | unknown[]>(
    path: fs.PathLike | number,
    options?: {encoding?: null | BufferEncoding; flag?: string | undefined},
  ) => Promise<T>;
};

const copiedMethods: Partial<Pick<typeof fs, ImportedMethods>> = {};
for (const methodName of importedMethods) {
  // @ts-ignore
  copiedMethods[methodName] = fs[methodName];
}

const promisifiedMethods: Partial<AsyncMethods> = {};
for (const [methodName, method] of Object.entries(copiedMethods)) {
  // @ts-ignore
  promisifiedMethods[methodName] = util.promisify(method);
}

const asyncMethods: AsyncMethods = {
  ...(promisifiedMethods as Omit<AsyncMethods, 'exists' | 'readFile'>),

  exists: (path: fs.PathLike) =>
    new Promise(resolve => {
      fs.access(path, fs.constants.F_OK, err =>
        err ? resolve(false) : resolve(true),
      );
    }),

  readFile: Object.assign(
    (util.promisify(
      fs.readFile,
    ) as unknown) as typeof fs.readFile.__promisify__,
    {
      asJson: async (
        path: fs.PathLike | number,
        options?: {encoding?: null | BufferEncoding; flag?: string | undefined},
      ) => {
        const opts = {
          encoding: options?.encoding || 'utf-8',
          flag: options?.flag,
        };
        return JSON.parse(await asyncMethods.readFile(path, opts));
      },
    },
  ),
};

export default asyncMethods;
