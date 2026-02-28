/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Beachball change file detection and version bump invocation.
 *
 * @format
 */
/**
 * Check whether there are pending beachball change files in the repo.
 *
 * Beachball stores change files as JSON in the `change/` directory at the
 * repo root. If there are any .json files there, there are pending changes.
 */
export declare function hasChangeFiles(repoRoot: string): boolean;
/**
 * Run beachball bump to consume change files and update versions/changelogs.
 *
 * Invokes: npx beachball bump --branch <remote>/<target> --yes --verbose
 *
 * The --yes flag suppresses prompts.
 * The --verbose flag provides detailed output.
 * The --branch flag tells beachball the baseline branch to diff against.
 */
export declare function bumpVersions(opts: {
    targetBranch: string;
    remote: string;
    cwd: string;
}): Promise<void>;
