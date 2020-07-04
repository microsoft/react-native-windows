/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as crypto from 'crypto';
import * as fs from 'fs';
import * as os from 'os';
import * as path from 'path';

import GitReactFileRepository from '../GitReactFileRepository';
import {OverrideFileRepository} from '../FileRepository';
import OverrideFileRepositoryImpl from '../OverrideFileRepositoryImpl';
import {getNpmPackage} from '../PackageUtils';

/**
 * Helper to acquire a scratch directory which will be deleted after function
 * exit
 */
export async function usingScratchDirectory<T>(
  fn: (dir: string) => Promise<T>,
) {
  const dir = path.join(
    os.tmpdir(),
    (await getNpmPackage()).name,
    'e2etest',
    crypto.randomBytes(16).toString('hex'),
  );

  try {
    return await fn(dir);
  } finally {
    await fs.promises.rmdir(dir, {recursive: true});
  }
}

/**
 * Helper to acquire an isolated GitReactFileRepository
 */
export async function usingGitReactRepo<T>(
  fn: (gitReactRepo: GitReactFileRepository) => Promise<T>,
) {
  return await usingScratchDirectory(async dir => {
    const gitReactRepo = await GitReactFileRepository.createAndInit(dir);
    return await fn(gitReactRepo);
  });
}

/**
 * Helper to create an isolated override repository and clean it up once execution finishes
 */
export async function usingOverrideRepo<T>(
  overridesToCopy: string[],
  fn: (overrideRepo: OverrideFileRepository, baseDir: string) => Promise<T>,
): Promise<T> {
  return await usingScratchDirectory(async targetDirectory => {
    const overridePath = path.join(__dirname, 'collateral');

    for (const override of overridesToCopy) {
      const src = path.join(overridePath, override);
      const dst = path.join(targetDirectory, override);

      await fs.promises.mkdir(path.dirname(dst), {recursive: true});
      await fs.promises.copyFile(src, dst);
    }

    return await fn(
      new OverrideFileRepositoryImpl(targetDirectory),
      targetDirectory,
    );
  });
}
