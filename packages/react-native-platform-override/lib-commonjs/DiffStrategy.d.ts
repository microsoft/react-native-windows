/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import GitReactFileRepository from './GitReactFileRepository';
import { WritableFileRepository } from './FileRepository';
/**
 * An UpgradeStrategy describes the process to diff an override to its base
 */
export default interface DiffStrategy {
    diff(gitReactRepo: GitReactFileRepository, overrideRepo: WritableFileRepository): Promise<string>;
}
export declare const DiffStrategies: {
    /**
     * Assume the override is the same as the original
     */
    asssumeSame: () => DiffStrategy;
    /**
     * Assume the override is the same as the original
     */
    compareBaseFile: (overrideFile: string, baseFile: string, baseVersion: string) => DiffStrategy;
};
