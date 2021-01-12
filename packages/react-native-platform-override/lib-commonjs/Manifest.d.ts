/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as Serialized from './Serialized';
import Override from './Override';
import { ReactFileRepository, WritableFileRepository } from './FileRepository';
import OverrideFactory from './OverrideFactory';
import { ValidationError } from './ValidationStrategy';
/**
 * Represents a collection of overrides listed in an on-disk manifest. Allows
 * performing aggregate operations on the overrides.
 */
export default class Manifest {
    private includePatterns?;
    private excludePatterns?;
    private overrides;
    /**
     * Construct the manifest
     *
     * @param overrides List of overrides to evaluate
     * @param opts Allows specifying globs to include or exclude paths to enforce
     * exist in the manifest
     */
    constructor(overrides: Override[], opts?: {
        includePatterns?: string[];
        excludePatterns?: string[];
    });
    static fromSerialized(man: Serialized.Manifest): Manifest;
    /**
     * Check that overrides are accurately accounted for in the manifest. E.g.
     * all files should be accounted for, and base files should be up to date
     * with upstream.
     */
    validate(overrideRepo: WritableFileRepository, reactRepo: ReactFileRepository): Promise<ValidationError[]>;
    /**
     * Add an override to the manifest
     */
    addOverride(override: Override): void;
    /**
     * Whether the manifest contains a given override
     */
    hasOverride(overrideName: string): boolean;
    /**
     * Try to remove an override.
     * @returns false if none is found with the given name
     */
    removeOverride(overrideName: string): boolean;
    /**
     * Returns the entry corresponding to the given override path, or null if none
     * exists.
     */
    findOverride(overrideName: string): Override | null;
    /**
     * Updates an override entry to mark it as up-to-date in regards to its
     * current base file.
     */
    markUpToDate(overrideName: string, overrideFactory: OverrideFactory): Promise<void>;
    /**
     * Return a serialized representation of the manifest
     */
    serialize(): Serialized.Manifest;
    /**
     * Returns the overrides in the manfest
     */
    listOverrides(): Override[];
    /**
     * Find the index to a given override.
     * @returns -1 if it cannot be found
     */
    private findOverrideIndex;
}
