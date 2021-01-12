/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="node" />
import FileRepository from './FileRepository';
/**
 * Creates a hash from content, attempting to normalize for line-feeds
 */
export declare class Hasher {
    private hash;
    constructor();
    feedContent(content: string | Buffer): Hasher;
    digest(): string;
}
/**
 * Convenience helper which hashes a single buffer
 */
export declare function hashContent(content: string | Buffer): string;
/**
 * Helper to hash a file/directory belonging to a repository
 */
export declare function hashFileOrDirectory(name: string, repo: FileRepository): Promise<string | null>;
