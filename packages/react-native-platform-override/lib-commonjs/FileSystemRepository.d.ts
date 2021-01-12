/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="node" />
import { WritableFileRepository } from './FileRepository';
/**
 * Allows reading phsyical files based on a passed in directory
 */
export default class FileSystemRepository implements WritableFileRepository {
    private baseDir;
    constructor(baseDir: string);
    listFiles(globs?: string[]): Promise<string[]>;
    readFile(filename: string): Promise<Buffer | null>;
    stat(filename: string): Promise<'file' | 'directory' | 'none'>;
    writeFile(filename: string, content: Buffer | string): Promise<void>;
    deleteFile(filename: string): Promise<void>;
}
