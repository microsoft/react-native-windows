/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="node" />
import { VersionedReactFileRepository } from './FileRepository';
/**
 * Retrives React Native files using the React Native Github repo. Switching
 * between getting file contents of different versions may be slow.
 */
export default class GitReactFileRepository implements VersionedReactFileRepository {
    private fileRepo;
    private gitClient;
    private checkedOutVersion?;
    private batchingQueue;
    private constructor();
    static createAndInit(gitDirectory?: string): Promise<GitReactFileRepository>;
    listFiles(globs: string[] | undefined, reactNativeVersion: string): Promise<string[]>;
    readFile(filename: string, reactNativeVersion: string): Promise<Buffer | null>;
    stat(filename: string, reactNativeVersion: string): Promise<'file' | 'directory' | 'none'>;
    /**
     * Generate a Git-style patch to transform the given file into the given
     * content.
     */
    generatePatch(filename: string, reactNativeVersion: string, newContent: Buffer): Promise<string>;
    /**
     * Apply a patch to the given file, returning the merged result, which may
     * include conflict markers. The underlying file is not mutated.
     *
     * Git is unable to generate a representation with conflict markers in the
     * event of binary merge conflicts. In this case a null Buffer is returned.
     */
    getPatchedFile(filename: string, reactNativeVersion: string, patchContent: string): Promise<{
        patchedFile: Buffer | null;
        hasConflicts: boolean;
    }>;
    private usingVersion;
    private checkoutVersion;
    private tryCheckoutLocal;
    private fetchAndCheckout;
    private refFromVersion;
    private longCommitHash;
    private static defaultGitDirectory;
}
