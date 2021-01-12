/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import { ReactFileRepository, WritableFileRepository } from './FileRepository';
export interface ValidationError {
    /**
     * Why did validation fail?
     */
    type: 'missingFromManifest' | 'overrideNotFound' | 'baseNotFound' | 'expectedFile' | 'expectedDirectory' | 'outOfDate' | 'overrideDifferentFromBase' | 'overrideSameAsBase';
    /**
     * What override failed validation
     */
    overrideName: string;
}
/**
 * A ValidationStrategy allows describing a process to ensure a specific
 * override meets constraints such as ensuring file existence, preventing
 * content modification, or being up to date.
 */
export default interface ValidationStrategy {
    validate(overrideRepo: WritableFileRepository, reactRepo: ReactFileRepository): Promise<ValidationError[]>;
}
export declare const ValidationStrategies: {
    /**
     * Validate that an override file exists
     */
    overrideFileExists: (overrideName: string) => ValidationStrategy;
    /**
     * Validate that a base file exists
     */
    baseFileExists: (overrideName: string, baseFile: string) => ValidationStrategy;
    /**
     * Validate that an override directory exists
     */
    overrideDirectoryExists: (overrideName: string) => ValidationStrategy;
    /**
     * Validate that a base directory exists
     */
    baseDirectoryExists: (overrideName: string, baseDirectory: string) => ValidationStrategy;
    /**
     * Validate that a base file/folder matches an expected hash if it exists
     */
    baseUpToDate: (overrideName: string, base: string, expectedBaseHash: string) => ValidationStrategy;
    /**
     * Validate that an override meant to be a copy of a base file/folder has not
     * been tampered with
     */
    overrideCopyOfBase: (overrideName: string, base: string) => ValidationStrategy;
    /**
     * Validate that an override assumed to be different from its base is not
     * identical.
     */
    overrideDifferentFromBase: (overrideName: string, base: string) => ValidationStrategy;
};
