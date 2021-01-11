/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {exec, ExecOptions} from 'child_process';
import findRepoRoot from '@react-native-windows/find-repo-root';

/**
 * Run a command asyncronously, setting cwd to repo root if none is specified
 */
export default async function runCommand(
  command: string,
  opts: ExecOptions = {},
): Promise<{stdout: string; stderr: string}> {
  if (!opts.cwd) {
    opts.cwd = await findRepoRoot();
  }

  return new Promise((resolve, reject) => {
    exec(command, opts, (err, stdout, stderr) => {
      if (err) {
        reject(err);
      } else {
        resolve({stdout, stderr});
      }
    });
  });
}
