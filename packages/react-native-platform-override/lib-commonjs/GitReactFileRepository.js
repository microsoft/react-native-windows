"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const os = require("os");
const path = require("path");
const semver = require("semver");
const simplegit = require("simple-git/promise");
const BatchingQueue_1 = require("./BatchingQueue");
const FileSystemRepository_1 = require("./FileSystemRepository");
const node_fetch_1 = require("node-fetch");
const PackageUtils_1 = require("./PackageUtils");
const RN_COMMIT_ENDPOINT = 'https://api.github.com/repos/facebook/react-native/commits';
const RN_GITHUB_URL = 'https://github.com/facebook/react-native.git';
/**
 * Retrives React Native files using the React Native Github repo. Switching
 * between getting file contents of different versions may be slow.
 */
class GitReactFileRepository {
    constructor(gitDirectory, gitClient) {
        this.batchingQueue = new BatchingQueue_1.default();
        this.fileRepo = new FileSystemRepository_1.default(gitDirectory);
        this.gitClient = gitClient;
    }
    static async createAndInit(gitDirectory) {
        const dir = gitDirectory || (await this.defaultGitDirectory());
        await fs.promises.mkdir(dir, { recursive: true });
        const gitClient = simplegit(dir);
        gitClient.silent(true);
        if (!(await gitClient.checkIsRepo())) {
            await gitClient.init();
        }
        return new GitReactFileRepository(dir, gitClient);
    }
    async listFiles(globs, reactNativeVersion) {
        return this.usingVersion(reactNativeVersion, () => this.fileRepo.listFiles(globs));
    }
    async readFile(filename, reactNativeVersion) {
        return this.usingVersion(reactNativeVersion, () => this.fileRepo.readFile(filename));
    }
    async stat(filename, reactNativeVersion) {
        return this.usingVersion(reactNativeVersion, () => this.fileRepo.stat(filename));
    }
    /**
     * Generate a Git-style patch to transform the given file into the given
     * content.
     */
    async generatePatch(filename, reactNativeVersion, newContent) {
        return this.usingVersion(reactNativeVersion, async () => {
            try {
                await this.fileRepo.writeFile(filename, newContent);
                const patch = await this.gitClient.diff([
                    '--patch',
                    '--ignore-space-at-eol',
                    '--binary',
                ]);
                if (patch.length === 0) {
                    throw new Error(`Generated patch for ${filename} was empty. Is it identical to the original?`);
                }
                return patch;
            }
            finally {
                await this.gitClient.reset('hard');
            }
        });
    }
    /**
     * Apply a patch to the given file, returning the merged result, which may
     * include conflict markers. The underlying file is not mutated.
     *
     * Git is unable to generate a representation with conflict markers in the
     * event of binary merge conflicts. In this case a null Buffer is returned.
     */
    async getPatchedFile(filename, reactNativeVersion, patchContent) {
        return this.usingVersion(reactNativeVersion, async () => {
            try {
                await this.fileRepo.writeFile('rnwgit.patch', patchContent);
                let hasConflicts = false;
                let binaryConflicts = false;
                try {
                    await this.gitClient.raw([
                        'apply',
                        '--3way',
                        '--whitespace=nowarn',
                        'rnwgit.patch',
                    ]);
                }
                catch (ex) {
                    // Hack alert: simple-git doesn't populate exception information from
                    // conflicts when we're using raw commands (which we need to since it
                    // doesn't support apply). Try to detect if Git gave us a bad exit code
                    // because of merge conflicts, which we explicitly want to allow.
                    if (!ex.message.includes('with conflicts')) {
                        throw ex;
                    }
                    hasConflicts = true;
                    binaryConflicts = ex.message.includes('Cannot merge binary files');
                }
                const patchedFile = binaryConflicts
                    ? null
                    : await this.fileRepo.readFile(filename);
                return { patchedFile, hasConflicts };
            }
            finally {
                await this.gitClient.reset('hard');
            }
        });
    }
    async usingVersion(reactNativeVersion, fn) {
        return await this.batchingQueue.enqueue(reactNativeVersion, async () => {
            await this.checkoutVersion(reactNativeVersion);
            return await fn();
        });
    }
    async checkoutVersion(reactNativeVersion) {
        if (reactNativeVersion !== this.checkedOutVersion) {
            if (!(await this.tryCheckoutLocal(reactNativeVersion))) {
                await this.fetchAndCheckout(reactNativeVersion);
            }
            this.checkedOutVersion = reactNativeVersion;
        }
    }
    async tryCheckoutLocal(reactNativeVersion) {
        try {
            await this.gitClient.checkout([reactNativeVersion, '--force']);
            return true;
        }
        catch (_a) {
            return false;
        }
    }
    async fetchAndCheckout(reactNativeVersion) {
        const gitRef = await this.refFromVersion(reactNativeVersion);
        try {
            await this.gitClient.fetch([
                RN_GITHUB_URL,
                `${gitRef}:${reactNativeVersion}`,
                '--depth=1',
            ]);
        }
        catch (ex) {
            throw new Error(`Failed to fetch '${gitRef}'. Does it exist? (${ex.message})`);
        }
        await this.gitClient.checkout([reactNativeVersion, '--force']);
    }
    async refFromVersion(reactNativeVersion) {
        if (!semver.valid(reactNativeVersion)) {
            throw new Error(`${reactNativeVersion} is not a valid semver version`);
        }
        // Stable releases of React Native use a tag where nightly releases embed
        // a commit hash into the prerelease tag of 0.0.0 versions
        if (semver.lt(reactNativeVersion, '0.0.0', { includePrerelease: true })) {
            // We cannot do a shallow fetch of an abbreviated commit hash
            const shortHash = semver.prerelease(reactNativeVersion)[0];
            return this.longCommitHash(shortHash);
        }
        else {
            return `refs/tags/v${reactNativeVersion}`;
        }
    }
    async longCommitHash(shortHash) {
        // We cannot get long hash directly from a remote, so query Github's API
        // for it.
        const commitInfo = await node_fetch_1.default(`${RN_COMMIT_ENDPOINT}/${shortHash}`);
        if (!commitInfo.ok) {
            throw new Error(`Unable to query Github for commit '${shortHash}`);
        }
        return (await commitInfo.json()).sha;
    }
    static async defaultGitDirectory() {
        return path.join(os.tmpdir(), (await PackageUtils_1.getNpmPackage()).name, 'git');
    }
}
exports.default = GitReactFileRepository;
//# sourceMappingURL=GitReactFileRepository.js.map