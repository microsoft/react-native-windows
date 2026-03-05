"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Async wrapper around Node.js child_process.spawn.
 * Simplified from fork-sync/src/modules/proc.ts.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.exec = exports.spawn = exports.ExecError = void 0;
const child_process_1 = require("child_process");
/**
 * Error thrown when a spawned process exits with a non-zero code.
 */
class ExecError extends Error {
    constructor(opts) {
        const cmdStr = [opts.command, ...opts.args].join(' ');
        super(opts.stderr ||
            `Command failed with exit code ${opts.exitCode}: ${cmdStr}`);
        this.name = 'ExecError';
        this.command = opts.command;
        this.args = opts.args;
        this.cwd = opts.cwd;
        this.exitCode = opts.exitCode;
        this.stderr = opts.stderr;
    }
}
exports.ExecError = ExecError;
/**
 * Spawn a command (no shell) and return its trimmed stdout.
 * Throws ExecError on non-zero exit unless `fallback` is provided.
 */
function spawn(command, args, opts) {
    return spawnImpl(command, [...args], opts, false);
}
exports.spawn = spawn;
/**
 * Execute a command string in a shell and return its trimmed stdout.
 * Uses shell mode, needed for .cmd shims on Windows (npx, gh).
 */
function exec(command, opts) {
    return spawnImpl(command, [], opts, true);
}
exports.exec = exec;
function spawnImpl(command, args, opts, shell) {
    return new Promise((resolve, reject) => {
        const child = (0, child_process_1.spawn)(command, args, {
            cwd: opts === null || opts === void 0 ? void 0 : opts.cwd,
            env: (opts === null || opts === void 0 ? void 0 : opts.env) ? { ...process.env, ...opts.env } : undefined,
            stdio: ['ignore', 'pipe', 'pipe'],
            windowsHide: true,
            shell,
        });
        const stdoutChunks = [];
        const stderrChunks = [];
        child.stdout.on('data', (chunk) => stdoutChunks.push(chunk));
        child.stderr.on('data', (chunk) => stderrChunks.push(chunk));
        child.on('error', err => {
            reject(err);
        });
        child.on('close', exitCode => {
            const stdout = Buffer.concat(stdoutChunks).toString('utf8').trimEnd();
            const stderr = Buffer.concat(stderrChunks).toString('utf8').trimEnd();
            if (exitCode !== 0) {
                if ((opts === null || opts === void 0 ? void 0 : opts.fallback) !== undefined) {
                    resolve(opts.fallback);
                }
                else {
                    reject(new ExecError({
                        command,
                        args,
                        cwd: opts === null || opts === void 0 ? void 0 : opts.cwd,
                        exitCode,
                        stderr,
                    }));
                }
            }
            else {
                resolve(stdout);
            }
        });
    });
}
//# sourceMappingURL=proc.js.map