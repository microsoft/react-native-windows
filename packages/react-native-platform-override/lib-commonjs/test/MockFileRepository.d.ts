/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/// <reference types="node" />
import FileRepository, { ReactFileRepository, WritableFileRepository } from '../FileRepository';
export interface MockFile {
    filename: string;
    content: string | Buffer;
}
export interface MockEmptyDirectory {
    dirname: string;
}
export default class MockFileRepository implements FileRepository {
    protected files: MockFile[];
    protected directories: MockEmptyDirectory[];
    constructor(files: MockFile[], directories?: MockEmptyDirectory[]);
    listFiles(globs?: string[]): Promise<string[]>;
    readFile(filename: string): Promise<Buffer | null>;
    stat(filename: string): Promise<'file' | 'directory' | 'none'>;
}
export declare class MockReactFileRepository extends MockFileRepository implements ReactFileRepository {
    getVersion(): string;
}
export declare class MockWritableFileRepository extends MockFileRepository implements WritableFileRepository {
    writeFile(filename: string, content: Buffer): Promise<void>;
    deleteFile(filename: string): Promise<void>;
}
