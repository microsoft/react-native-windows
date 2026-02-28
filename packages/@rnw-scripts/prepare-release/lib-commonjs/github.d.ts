/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * GitHub operations via the gh CLI.
 *
 * @format
 */
export interface PRInfo {
    number: number;
    url: string;
}
/**
 * Find an existing open PR by head branch name.
 * Returns null if no matching PR exists.
 */
export declare function findPR(opts: {
    head: string;
    cwd: string;
    repo?: string;
}): Promise<PRInfo | null>;
/**
 * Create a new pull request. Returns the PR number and URL.
 *
 * Uses --body-file with a temp file to avoid shell escaping issues
 * with multiline markdown content on Windows cmd.exe.
 */
export declare function createPR(opts: {
    head: string;
    base: string;
    title: string;
    body: string;
    cwd: string;
    repo?: string;
}): Promise<PRInfo>;
/**
 * Update an existing pull request's body text.
 *
 * Uses --body-file with a temp file to avoid shell escaping issues
 * with multiline markdown content on Windows cmd.exe.
 */
export declare function updatePR(opts: {
    number: number;
    body: string;
    cwd: string;
    repo?: string;
}): Promise<void>;
