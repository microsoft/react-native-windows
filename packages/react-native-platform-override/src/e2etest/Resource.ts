/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as crypto from 'crypto';
import * as fs from 'fs';
import * as globby from 'globby';
import * as os from 'os';
import * as path from 'path';

import FileSystemRepository from '../FileSystemRepository';
import GitReactFileRepository from '../GitReactFileRepository';
import {WritableFileRepository} from '../FileRepository';
import {getNpmPackage} from '../PackageUtils';

/**
 * Helper to acquire a scratch directory which must be deleted using the
 * returned callback.
 */
export async function acquireSratchDirectory(): Promise<
  [string, () => Promise<void>]
> {
  const dir = path.join(
    os.tmpdir(),
    (await getNpmPackage()).name,
    'e2etest',
    crypto.randomBytes(16).toString('hex'),
  );
  await fs.promises.mkdir(dir, {recursive: true});
  return [dir, async () => fs.promises.rmdir(dir, {recursive: true})];
}

/**
 * Helper to acquire a scratch directory which will be deleted after function
 * exit
 */
export async function usingScratchDirectory<T>(
  fn: (dir: string) => Promise<T>,
) {
  return usingAcquired(fn, await acquireSratchDirectory());
}

/**
 * Helper to acquire an isolated GitReactFileRepository whuch ust be deleted
 * using the returned callback.
 */
export async function acquireGitRepo<T>(): Promise<
  [GitReactFileRepository, () => Promise<void>]
> {
  const [dir, dispose] = await acquireSratchDirectory();
  const gitReactRepo = await GitReactFileRepository.createAndInit(dir);
  return [gitReactRepo, dispose];
}

/**
 * Helper to acquire an isolated GitReactFileRepository which will be deleted
 * after function exit
 */
export async function usingGitReactRepo<T>(
  fn: (gitReactRepo: GitReactFileRepository) => Promise<T>,
) {
  return usingAcquired(fn, await acquireGitRepo());
}

/**
 * Helper to create an isolated repository filled with specified files and
 * clean it up once execution finishes
 */
export async function usingFiles<T>(
  overridesToCopy: string[],
  fn: (overrideRepo: WritableFileRepository, baseDir: string) => Promise<T>,
): Promise<T> {
  return await usingScratchDirectory(async targetDirectory => {
    const collateralPath = path.join(__dirname, 'collateral');

    await Promise.all(
      overridesToCopy.map(async override => {
        const src = path.join(collateralPath, override);
        const dst = path.join(targetDirectory, override);

        await fs.promises.mkdir(path.dirname(dst), {recursive: true});
        return await fs.promises.copyFile(src, dst);
      }),
    );

    return await fn(new FileSystemRepository(targetDirectory), targetDirectory);
  });
}

/**
 * Helper to create an isolated repository cloned from a source folder and
 * clean it up once execution finishes
 */
export async function usingRepository<T>(
  sourceFolder: string,
  fn: (overrideRepo: WritableFileRepository, baseDir: string) => Promise<T>,
): Promise<T> {
  const collateralPath = path.join(__dirname, 'collateral');
  const srcRepo = path.join(collateralPath, sourceFolder);
  const srcFiles = (
    await globby(['**/*'], {cwd: srcRepo, absolute: true})
  ).map(f => path.relative(collateralPath, f));

  return await usingFiles(srcFiles, async (repo, baseDir) => {
    return await fn(repo, path.join(baseDir, sourceFolder));
  });
}

/**
 * Pass an acquired resource to a function and dispose of it after function
 * exit
 */
async function usingAcquired<T, U>(
  fn: (resource: T) => Promise<U>,
  acquired: [T, () => Promise<void>],
): Promise<U> {
  try {
    return await fn(acquired[0]);
  } finally {
    await acquired[1]();
  }
}
