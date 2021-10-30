/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 */

import crypto from 'crypto';
import fs from 'fs';
import os from 'os';
import path from 'path';

/**
 * Resource, and a callback to dispose of it
 */
export type Disposable<T> = [T, () => Promise<void>];

/**
 * Helper to acquire a scratch directory which must be deleted using the
 * returned callback.
 */
export async function acquireSratchDirectory(): Promise<Disposable<string>> {
  const dir = path.join(
    os.tmpdir(),
    '@react-native-windows/cli',
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
 * Pass an acquired resource to a function and dispose of it after function
 * exit
 */
async function usingAcquired<T, U>(
  fn: (resource: T) => Promise<U>,
  acquired: Disposable<T>,
): Promise<U> {
  try {
    return await fn(acquired[0]);
  } finally {
    await acquired[1]();
  }
}
