/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Async wrapper around Node.js child_process.spawn.
 * Simplified from fork-sync/src/modules/proc.ts.
 *
 * @format
 */

import {spawn as nodeSpawn} from 'child_process';

/**
 * Error thrown when a spawned process exits with a non-zero code.
 */
export class ExecError extends Error {
  readonly command: string;
  readonly args: readonly string[];
  readonly cwd: string | undefined;
  readonly exitCode: number | null;
  readonly stderr: string;

  constructor(opts: {
    command: string;
    args: readonly string[];
    cwd?: string;
    exitCode: number | null;
    stderr: string;
  }) {
    const cmdStr = [opts.command, ...opts.args].join(' ');
    super(
      opts.stderr ||
        `Command failed with exit code ${opts.exitCode}: ${cmdStr}`,
    );
    this.name = 'ExecError';
    this.command = opts.command;
    this.args = opts.args;
    this.cwd = opts.cwd;
    this.exitCode = opts.exitCode;
    this.stderr = opts.stderr;
  }
}

export interface SpawnOpts {
  cwd?: string;
  /** If set, return this value instead of throwing on non-zero exit */
  fallback?: string;
  /** Extra environment variables (merged with process.env) */
  env?: Record<string, string>;
}

/**
 * Spawn a command (no shell) and return its trimmed stdout.
 * Throws ExecError on non-zero exit unless `fallback` is provided.
 */
export function spawn(
  command: string,
  args: readonly string[],
  opts?: SpawnOpts,
): Promise<string> {
  return spawnImpl(command, [...args], opts, false);
}

/**
 * Execute a command string in a shell and return its trimmed stdout.
 * Uses shell mode, needed for .cmd shims on Windows (npx, gh).
 */
export function exec(command: string, opts?: SpawnOpts): Promise<string> {
  return spawnImpl(command, [], opts, true);
}

function spawnImpl(
  command: string,
  args: string[],
  opts: SpawnOpts | undefined,
  shell: boolean,
): Promise<string> {
  return new Promise<string>((resolve, reject) => {
    const child = nodeSpawn(command, args, {
      cwd: opts?.cwd,
      env: opts?.env ? {...process.env, ...opts.env} : undefined,
      stdio: ['ignore', 'pipe', 'pipe'],
      windowsHide: true,
      shell,
    });

    const stdoutChunks: Buffer[] = [];
    const stderrChunks: Buffer[] = [];

    child.stdout!.on('data', (chunk: Buffer) => stdoutChunks.push(chunk));
    child.stderr!.on('data', (chunk: Buffer) => stderrChunks.push(chunk));

    child.on('error', err => {
      reject(err);
    });

    child.on('close', exitCode => {
      const stdout = Buffer.concat(stdoutChunks).toString('utf8').trimEnd();
      const stderr = Buffer.concat(stderrChunks).toString('utf8').trimEnd();

      if (exitCode !== 0) {
        if (opts?.fallback !== undefined) {
          resolve(opts.fallback);
        } else {
          reject(
            new ExecError({
              command,
              args,
              cwd: opts?.cwd,
              exitCode,
              stderr,
            }),
          );
        }
      } else {
        resolve(stdout);
      }
    });
  });
}
