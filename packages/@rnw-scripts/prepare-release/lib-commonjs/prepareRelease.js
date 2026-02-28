"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * This script automates the version bump PR workflow. It checks for pending
 * beachball change files, bumps versions, and creates or updates a
 * "Version Packages" pull request.
 *
 * Usage:
 *   npx prepare-release --branch <target> [--dry-run] [--no-color] [--help]
 *
 * @format
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const node_util_1 = require("node:util");
const fs_1 = __importDefault(require("fs"));
const path_1 = __importDefault(require("path"));
const find_repo_root_1 = __importDefault(require("@react-native-windows/find-repo-root"));
const git_1 = require("./git");
const github_1 = require("./github");
const beachballBump_1 = require("./beachballBump");
const releaseSummary_1 = require("./releaseSummary");
// ---------------------------------------------------------------------------
// Color utilities (from npmPack.js pattern)
// ---------------------------------------------------------------------------
const ansi = {
    reset: '\x1b[0m',
    bright: '\x1b[1m',
    dim: '\x1b[2m',
    red: '\x1b[31m',
    green: '\x1b[32m',
    yellow: '\x1b[33m',
    blue: '\x1b[34m',
    cyan: '\x1b[36m',
    gray: '\x1b[90m',
};
let useColors = true;
function colorize(text, color) {
    if (!useColors) {
        return text;
    }
    return color + text + ansi.reset;
}
// ---------------------------------------------------------------------------
// CLI help
// ---------------------------------------------------------------------------
function showHelp() {
    console.log(`
prepare-release - Automate version bump PRs using beachball

Usage:
  npx prepare-release --branch <target> [options]

Options:
  --branch <name>   Target branch to prepare release for (required)
  --dry-run         Do everything except push and PR create/update
  --no-color        Disable colored output
  --help, -h        Show this help message

Examples:
  npx prepare-release --branch main
  npx prepare-release --branch 0.76-stable --dry-run
`);
}
// ---------------------------------------------------------------------------
// Remote detection
// ---------------------------------------------------------------------------
/**
 * Normalize a git URL to a canonical form for comparison.
 *
 * Handles SSH (git@github.com:org/repo.git), HTTPS, with/without .git suffix.
 * Output: lowercase "github.com/org/repo"
 */
function normalizeGitUrl(url) {
    let normalized = url;
    // Strip trailing .git
    normalized = normalized.replace(/\.git$/, '');
    // Convert SSH format: git@github.com:org/repo -> github.com/org/repo
    normalized = normalized.replace(/^git@([^:]+):/, '$1/');
    // Convert HTTPS format: https://github.com/org/repo -> github.com/org/repo
    normalized = normalized.replace(/^https?:\/\//, '');
    // Lowercase for case-insensitive comparison
    normalized = normalized.toLowerCase();
    // Strip trailing slash
    normalized = normalized.replace(/\/$/, '');
    return normalized;
}
/**
 * Extract "owner/repo" from a git URL for use with `gh --repo`.
 *
 * E.g. "git@github.com:microsoft/react-native-windows.git"
 *   -> "microsoft/react-native-windows"
 */
function extractGitHubRepo(url) {
    const normalized = normalizeGitUrl(url);
    // normalized is like "github.com/owner/repo"
    const match = normalized.match(/github\.com\/(.+)/);
    if (!match) {
        throw new Error(`Could not extract GitHub owner/repo from "${url}"`);
    }
    return match[1];
}
/**
 * Detect which git remote matches the repository URL from package.json.
 *
 * Parses `git remote -v` output and matches against the normalized repo URL.
 * On CI this is typically "origin"; on developer machines it may differ.
 */
async function detectRemote(git, repoUrl) {
    const canonical = normalizeGitUrl(repoUrl);
    const remoteOutput = await git.remoteList();
    for (const line of remoteOutput.split('\n')) {
        // Lines: "origin\thttps://github.com/microsoft/react-native-windows.git (fetch)"
        const match = line.match(/^(\S+)\s+(\S+)\s+\(fetch\)/);
        if (!match) {
            continue;
        }
        const remoteName = match[1];
        const remoteUrl = match[2];
        if (normalizeGitUrl(remoteUrl) === canonical) {
            return remoteName;
        }
    }
    throw new Error(`Could not find a git remote matching "${repoUrl}". ` +
        'Run "git remote -v" and verify the repository URL in root package.json.');
}
// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
(async () => {
    var _a, _b, _c;
    // 1. Parse CLI arguments
    const { values } = (0, node_util_1.parseArgs)({
        options: {
            branch: { type: 'string' },
            'dry-run': { type: 'boolean', default: false },
            help: { type: 'boolean', short: 'h', default: false },
            'no-color': { type: 'boolean', default: false },
        },
    });
    if (values.help) {
        showHelp();
        process.exit(0);
    }
    useColors = !values['no-color'];
    const targetBranch = values.branch;
    if (!targetBranch) {
        console.error(colorize('Error: --branch is required', ansi.red));
        showHelp();
        process.exit(1);
    }
    const dryRun = (_a = values['dry-run']) !== null && _a !== void 0 ? _a : false;
    if (dryRun) {
        console.log(colorize('[DRY RUN MODE]', ansi.yellow));
    }
    try {
        // 2. Find repo root
        const repoRoot = await (0, find_repo_root_1.default)();
        console.log(`${colorize('Repository root:', ansi.bright)} ${repoRoot}`);
        // 3. Read repository URL from root package.json
        const rootPkgJsonPath = path_1.default.join(repoRoot, 'package.json');
        const rootPkgJson = JSON.parse(fs_1.default.readFileSync(rootPkgJsonPath, 'utf8'));
        const repoUrl = (_c = (_b = rootPkgJson.repository) === null || _b === void 0 ? void 0 : _b.url) !== null && _c !== void 0 ? _c : '';
        if (!repoUrl) {
            throw new Error('Could not find repository.url in root package.json');
        }
        console.log(`${colorize('Repository URL:', ansi.bright)} ${repoUrl}`);
        // 4. Detect git remote name
        const git = new git_1.GitRepo(repoRoot);
        const remoteName = await detectRemote(git, repoUrl);
        console.log(`${colorize('Git remote:', ansi.bright)} ${remoteName}`);
        // 4b. Extract GitHub owner/repo for gh CLI --repo flag
        const githubRepo = extractGitHubRepo(repoUrl);
        console.log(`${colorize('GitHub repo:', ansi.bright)} ${githubRepo}`);
        // 5. Fetch from remote
        console.log(colorize(`Fetching from ${remoteName}...`, ansi.dim));
        await git.fetch(remoteName);
        // 6. Check for pending change files
        console.log(colorize('Checking for change files...', ansi.dim));
        if (!(0, beachballBump_1.hasChangeFiles)(repoRoot)) {
            console.log(colorize('No pending change files found. Nothing to do.', ansi.green));
            process.exit(0);
        }
        console.log(colorize('Found pending change files.', ansi.green));
        // 7. Check for existing PR
        const prBranch = `prepare-release/${targetBranch}`;
        console.log(colorize(`Looking for existing PR from ${prBranch}...`, ansi.dim));
        const existingPR = await (0, github_1.findPR)({
            head: prBranch,
            cwd: repoRoot,
            repo: githubRepo,
        });
        if (existingPR) {
            console.log(`${colorize('Found existing PR:', ansi.bright)} #${existingPR.number} (${existingPR.url})`);
        }
        else {
            console.log(colorize('No existing PR found. Will create one.', ansi.dim));
        }
        // 8. Save original branch so we can restore it when done
        const originalBranch = await git.currentBranch();
        console.log(colorize(`Saving current branch: ${originalBranch}`, ansi.dim));
        try {
            // 9. Create/reset the prepare-release branch from target branch HEAD
            console.log(colorize(`Creating branch ${prBranch} from ${remoteName}/${targetBranch}...`, ansi.dim));
            await git.checkoutNewBranch(prBranch, `${remoteName}/${targetBranch}`);
            // 10. Run beachball bump
            console.log(colorize('Running beachball bump...', ansi.bright));
            await (0, beachballBump_1.bumpVersions)({
                targetBranch,
                remote: remoteName,
                cwd: repoRoot,
            });
            // 11. Check if beachball actually changed anything
            const status = await git.statusPorcelain();
            if (!status) {
                console.log(colorize('beachball bump made no changes. Nothing to commit.', ansi.yellow));
                process.exit(0);
            }
            // 12. Collect bumped package info for PR description
            //     Parse changed package.json paths from git status --porcelain output
            const changedFiles = status
                .split('\n')
                .map(line => line.trim().split(/\s+/).pop())
                .filter(f => f.endsWith('package.json'));
            const bumpedPackages = (0, releaseSummary_1.collectBumpedPackages)(changedFiles, repoRoot);
            console.log((0, releaseSummary_1.generateConsoleSummary)(bumpedPackages));
            // 13. Stage all + commit
            const commitMessage = `Version Packages (${targetBranch})`;
            console.log(colorize(`Committing: "${commitMessage}"...`, ansi.dim));
            await git.stageAll();
            await git.commit(commitMessage);
            // 14. Force-push the branch
            if (dryRun) {
                console.log(colorize(`[DRY RUN] Would force-push ${prBranch} to ${remoteName}`, ansi.yellow));
            }
            else {
                console.log(colorize(`Force-pushing ${prBranch} to ${remoteName}...`, ansi.dim));
                await git.push(remoteName, prBranch, { force: true });
                // Verify the branch landed on the remote
                const lsRemoteOut = await git.lsRemote(remoteName, prBranch);
                if (!lsRemoteOut) {
                    throw new Error(`Push verification failed: branch "${prBranch}" not found on ` +
                        `"${remoteName}" after push. Check your push permissions.`);
                }
                console.log(colorize(`Push verified: ${prBranch} exists on ${remoteName}`, ansi.green));
            }
            // 15. Create or update PR
            const prTitle = `Version Packages (${targetBranch})`;
            const prBody = (0, releaseSummary_1.generatePRBody)(targetBranch, bumpedPackages);
            if (existingPR) {
                if (dryRun) {
                    console.log(colorize(`[DRY RUN] Would update PR #${existingPR.number}`, ansi.yellow));
                }
                else {
                    console.log(colorize(`Updating PR #${existingPR.number}...`, ansi.dim));
                    await (0, github_1.updatePR)({
                        number: existingPR.number,
                        body: prBody,
                        cwd: repoRoot,
                        repo: githubRepo,
                    });
                    console.log(`${colorize('PR updated:', ansi.green)} ${existingPR.url}`);
                }
            }
            else {
                if (dryRun) {
                    console.log(colorize(`[DRY RUN] Would create PR: "${prTitle}"`, ansi.yellow));
                }
                else {
                    console.log(colorize('Creating pull request...', ansi.dim));
                    const newPR = await (0, github_1.createPR)({
                        head: prBranch,
                        base: targetBranch,
                        title: prTitle,
                        body: prBody,
                        cwd: repoRoot,
                        repo: githubRepo,
                    });
                    console.log(`${colorize('PR created:', ansi.green)} ${newPR.url}`);
                }
            }
            // 16. Done
            console.log('');
            console.log(colorize('Done!', ansi.green + ansi.bright));
            if (dryRun) {
                console.log(colorize('\nPR body that would be used:', ansi.dim));
                console.log(prBody);
            }
        }
        finally {
            // Always restore the original branch
            if (originalBranch && originalBranch !== 'HEAD') {
                console.log(colorize(`Restoring original branch: ${originalBranch}`, ansi.dim));
                await git.checkout(originalBranch);
            }
        }
    }
    catch (err) {
        const message = err instanceof Error ? err.message : String(err);
        console.error(`${colorize('Error:', ansi.red)} ${message}`);
        process.exit(1);
    }
})();
//# sourceMappingURL=prepareRelease.js.map