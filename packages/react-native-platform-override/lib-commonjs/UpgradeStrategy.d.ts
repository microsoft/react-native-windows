/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import { WritableFileRepository } from './FileRepository';
import GitReactFileRepository from './GitReactFileRepository';
export interface UpgradeResult {
    overrideName: string;
    filesWritten: boolean;
    hasConflicts: boolean;
}
/**
 * An UpgradeStrategy describes the process to upgrade an individual override
 * to a new version of the React Native source tree.
 */
export default interface UpgradeStrategy {
    upgrade(gitReactRepo: GitReactFileRepository, overrideRepo: WritableFileRepository, newVersion: string, allowConflicts: boolean): Promise<UpgradeResult>;
}
export declare const UpgradeStrategies: {
    /**
     * No work needed to upgrade
     */
    assumeUpToDate: (overrideName: string) => UpgradeStrategy;
    /**
     * Perform a three way merge of the original base file, the overriden version
     * of it, and the base file from a newwer version of React Native.
     */
    threeWayMerge: (overrideName: string, baseFile: string, baseVersion: string) => UpgradeStrategy;
    /**
     * Overwrite our override with base file contents
     */
    copyFile: (overrideName: string, baseFile: string) => UpgradeStrategy;
    /**
     * Overwrite our override with base file contents
     */
    copyDirectory: (overrideDirectory: string, baseDirectory: string) => UpgradeStrategy;
};
