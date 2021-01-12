/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import GitReactFileRepository from '../GitReactFileRepository';
import { WritableFileRepository } from '../FileRepository';
/**
 * Helper to acquire a scratch directory which must be deleted using the
 * returned callback.
 */
export declare function acquireSratchDirectory(): Promise<[string, () => Promise<void>]>;
/**
 * Helper to acquire a scratch directory which will be deleted after function
 * exit
 */
export declare function usingScratchDirectory<T>(fn: (dir: string) => Promise<T>): Promise<T>;
/**
 * Helper to acquire an isolated GitReactFileRepository whuch ust be deleted
 * using the returned callback.
 */
export declare function acquireGitRepo<T>(): Promise<[GitReactFileRepository, () => Promise<void>]>;
/**
 * Helper to acquire an isolated GitReactFileRepository which will be deleted
 * after function exit
 */
export declare function usingGitReactRepo<T>(fn: (gitReactRepo: GitReactFileRepository) => Promise<T>): Promise<T>;
/**
 * Helper to create an isolated repository filled with specified files and
 * clean it up once execution finishes
 */
export declare function usingFiles<T>(overridesToCopy: string[], fn: (overrideRepo: WritableFileRepository, baseDir: string) => Promise<T>): Promise<T>;
/**
 * Helper to create an isolated repository cloned from a source folder and
 * clean it up once execution finishes
 */
export declare function usingRepository<T>(sourceFolder: string, fn: (overrideRepo: WritableFileRepository, baseDir: string) => Promise<T>): Promise<T>;
