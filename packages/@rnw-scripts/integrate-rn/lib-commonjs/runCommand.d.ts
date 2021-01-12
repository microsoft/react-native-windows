/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="node" />
import { ExecOptions } from 'child_process';
/**
 * Run a command asyncronously, setting cwd to repo root if none is specified
 */
export default function runCommand(command: string, opts?: ExecOptions): Promise<{
    stdout: string;
    stderr: string;
}>;
