/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as async from 'async';
import * as path from 'path';

import {
  SpawnSyncOptions,
  StdioOptions,
  execSync,
  spawn,
  spawnSync,
} from 'child_process';

// @ts-ignore (no typings for clang-format)
import {getNativeBinary} from 'clang-format';

/// These constants control which files are formatted
const includeEndsWith = ['.h', '.cpp'];
const excludePathContains: string[] = [];
const excludePathEndsWith = ['.g.h', '.g.cpp'];

const VERIFY_FLAG = '-verify';

function main() {
  // Run clang-format.
  try {
    const verify = process.argv.indexOf(VERIFY_FLAG) > 0;
    const args = process.argv.slice(2).filter(_ => _ !== VERIFY_FLAG);

    // Pass all arguments to clang-format, including e.g. -version etc.
    spawnClangFormat(args, verify ? queryNoOpenFiles : process.exit, 'inherit');
  } catch (e) {
    process.stdout.write(e.message);
    process.exit(1);
  }
}

function queryNoOpenFiles() {
  const opened = execSync('git status -s').toString();
  if (opened) {
    console.error('The following files have incorrect formatting:');
    console.error(opened);
    console.error('Running `yarn format` from the repo root should fix this.');
    process.exit(2);
  }
}

function errorFromExitCode(exitCode: number) {
  return new Error(`clang-format exited with exit code ${exitCode}.`);
}

function git(args: string[], options: SpawnSyncOptions) {
  const results = spawnSync('git', args, options);

  if (results.status === 0) {
    return {
      stderr: results.stderr.toString().trim(),
      stdout: results.stdout.toString().trim(),
      success: true,
    };
  } else {
    return {
      stderr: results.stderr.toString().trim(),
      stdout: results.stdout.toString().trim(),
      success: false,
    };
  }
}

function listAllTrackedFiles(cwd: string) {
  const results = git(['ls-tree', '-r', '--name-only', '--full-tree', 'HEAD'], {
    cwd,
  });

  if (results.success) {
    return results.stdout.split('\n');
  }

  return [];
}

/**
 * Spawn the clang-format binary with given arguments.
 */
function spawnClangFormat(
  args: string[],
  done: (any?: any) => void,
  stdio: StdioOptions,
) {
  // WARNING: This function's interface should stay stable across versions for the cross-version
  // loading below to work.
  let nativeBinary: string;

  try {
    nativeBinary = getNativeBinary();
  } catch (e) {
    setImmediate(done.bind(e));
    return;
  }

  let files = listAllTrackedFiles(path.resolve(__dirname, '../..'));

  // Apply file filters from constants
  files = files.filter(
    file =>
      includeEndsWith.some(_ => file.endsWith(_)) &&
      !excludePathContains.some(_ => file.indexOf(_) > 0) &&
      !excludePathEndsWith.some(_ => file.endsWith(_)),
  );

  // split file array into chunks of 30
  let i: number;
  let j: number;
  const chunks = [];
  const chunkSize = 30;

  for (i = 0, j = files.length; i < j; i += chunkSize) {
    chunks.push(files.slice(i, i + chunkSize));
  }

  // launch a new process for each chunk
  async.series<number, Error>(
    chunks.map(chunk => {
      return function(callback) {
        const clangFormatProcess = spawn(nativeBinary, args.concat(chunk), {
          stdio: stdio,
        });
        clangFormatProcess.on('close', exit => {
          if (exit !== 0) {
            callback(errorFromExitCode(exit!));
          } else {
            callback();
          }
        });
      };
    }),
    err => {
      if (err) {
        done(err);
        return;
      }
      console.log('\n');
      console.log(
        `ran clang-format on ${files.length} ${
          files.length === 1 ? 'file' : 'files'
        }`,
      );
      done();
    },
  );
}

main();
