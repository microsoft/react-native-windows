/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Async wrapper around Node.js child_process.spawn.
 * Simplified from fork-sync/src/modules/proc.ts.
 *
 * @format
 */
/**
 * Error thrown when a spawned process exits with a non-zero code.
 */
export declare class ExecError extends Error {
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
    });
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
export declare function spawn(command: string, args: readonly string[], opts?: SpawnOpts): Promise<string>;
/**
 * Execute a command string in a shell and return its trimmed stdout.
 * Uses shell mode, needed for .cmd shims on Windows (npx, gh).
 */
export declare function exec(command: string, opts?: SpawnOpts): Promise<string>;
