/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Git operations module. Provides a typed wrapper around git CLI commands.
 * Simplified from fork-sync/src/modules/git.ts.
 *
 * @format
 */
/**
 * Typed wrapper around a git repository directory.
 * All methods execute git commands with cwd set to the wrapped directory.
 */
export declare class GitRepo {
    readonly dir: string;
    constructor(dir: string);
    /** Fetch from a remote. */
    fetch(remote: string): Promise<void>;
    /** Checkout an existing ref (branch, tag, or commit). */
    checkout(ref: string): Promise<void>;
    /**
     * Create or reset a branch to a start point.
     * Uses `git checkout -B` which creates the branch if it doesn't exist,
     * or resets it if it does.
     */
    checkoutNewBranch(name: string, startPoint?: string): Promise<void>;
    /** Stage all changes (git add --all). */
    stageAll(): Promise<void>;
    /** Create a commit with the given message. */
    commit(message: string): Promise<void>;
    /** Push a branch to a remote, optionally with --force. */
    push(remote: string, branch: string, opts?: {
        force?: boolean;
    }): Promise<void>;
    /** Resolve a ref to its SHA (git rev-parse). */
    revParse(ref: string): Promise<string>;
    /** Get the current branch name, or 'HEAD' if in detached state. */
    currentBranch(): Promise<string>;
    /** Check for uncommitted changes (git status --porcelain). */
    statusPorcelain(pathspec?: string): Promise<string>;
    /** List all remotes with their URLs (git remote -v). */
    remoteList(): Promise<string>;
    /** Git log with format and optional range. */
    log(opts: {
        format: string;
        range?: string;
    }): Promise<string>;
    /** Get file names changed between two refs, optionally filtered by path. */
    diffNameOnly(ref1: string, ref2?: string, pathspec?: string): Promise<string>;
    /** Check if a branch exists on a remote. Returns the ls-remote output or empty. */
    lsRemote(remote: string, branch: string): Promise<string>;
    /** Internal helper: run git with the repo's cwd. */
    private git;
}
