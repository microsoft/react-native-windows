"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Git operations module. Provides a typed wrapper around git CLI commands.
 * Simplified from fork-sync/src/modules/git.ts.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.GitRepo = void 0;
const proc_1 = require("./proc");
/**
 * Typed wrapper around a git repository directory.
 * All methods execute git commands with cwd set to the wrapped directory.
 */
class GitRepo {
    constructor(dir) {
        this.dir = dir;
    }
    /** Fetch from a remote. */
    async fetch(remote) {
        await this.git('fetch', remote);
    }
    /** Checkout an existing ref (branch, tag, or commit). */
    async checkout(ref) {
        await this.git('checkout', ref);
    }
    /**
     * Create or reset a branch to a start point.
     * Uses `git checkout -B` which creates the branch if it doesn't exist,
     * or resets it if it does.
     */
    async checkoutNewBranch(name, startPoint) {
        const args = ['checkout', '-B', name];
        if (startPoint) {
            args.push(startPoint);
        }
        await this.git(...args);
    }
    /** Stage all changes (git add --all). */
    async stageAll() {
        await this.git('add', '--all');
    }
    /** Create a commit with the given message. */
    async commit(message) {
        await this.git('commit', '-m', message);
    }
    /** Push a branch to a remote, optionally with --force. */
    async push(remote, branch, opts) {
        const args = ['push', remote, branch];
        if (opts === null || opts === void 0 ? void 0 : opts.force) {
            args.push('--force');
        }
        await this.git(...args);
    }
    /** Resolve a ref to its SHA (git rev-parse). */
    async revParse(ref) {
        return this.git('rev-parse', ref);
    }
    /** Get the current branch name, or 'HEAD' if in detached state. */
    async currentBranch() {
        return this.git('rev-parse', '--abbrev-ref', 'HEAD');
    }
    /** Check for uncommitted changes (git status --porcelain). */
    async statusPorcelain(pathspec) {
        const args = ['status', '--porcelain'];
        if (pathspec) {
            args.push('--', pathspec);
        }
        return this.git(...args);
    }
    /** List all remotes with their URLs (git remote -v). */
    async remoteList() {
        return this.git('remote', '-v');
    }
    /** Git log with format and optional range. */
    async log(opts) {
        const args = ['log', `--format=${opts.format}`];
        if (opts.range) {
            args.push(opts.range);
        }
        return this.git(...args);
    }
    /** Get file names changed between two refs, optionally filtered by path. */
    async diffNameOnly(ref1, ref2, pathspec) {
        const args = ['diff', '--name-only', ref1];
        if (ref2) {
            args.push(ref2);
        }
        if (pathspec) {
            args.push('--', pathspec);
        }
        return this.git(...args);
    }
    /** Check if a branch exists on a remote. Returns the ls-remote output or empty. */
    async lsRemote(remote, branch) {
        return this.git('ls-remote', '--heads', remote, `refs/heads/${branch}`);
    }
    /** Internal helper: run git with the repo's cwd. */
    async git(...args) {
        return (0, proc_1.spawn)('git', args, { cwd: this.dir });
    }
}
exports.GitRepo = GitRepo;
//# sourceMappingURL=git.js.map