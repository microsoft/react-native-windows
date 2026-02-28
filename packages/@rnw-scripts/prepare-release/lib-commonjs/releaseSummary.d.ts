/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Parse bumped packages and generate PR description markdown.
 *
 * @format
 */
export interface BumpedPackage {
    name: string;
    version: string;
    comments: Array<{
        comment: string;
        author: string;
    }>;
}
/**
 * Collect information about packages that were bumped by beachball.
 *
 * For each changed package.json, reads the new version and parses
 * CHANGELOG.json for the latest changelog entry.
 *
 * @param changedPackageJsonPaths Relative paths to package.json files
 *   that were modified by beachball bump (from git status --porcelain)
 * @param repoRoot The repository root directory
 */
export declare function collectBumpedPackages(changedPackageJsonPaths: string[], repoRoot: string): BumpedPackage[];
/**
 * Generate the pull request body markdown.
 */
export declare function generatePRBody(targetBranch: string, packages: BumpedPackage[]): string;
/**
 * Generate a console-friendly summary of bumped packages.
 */
export declare function generateConsoleSummary(packages: BumpedPackage[]): string;
