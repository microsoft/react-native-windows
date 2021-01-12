/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import OverrideFactory from './OverrideFactory';
import Override from './Override';
import { UpgradeResult } from './UpgradeStrategy';
import { ValidationError } from './ValidationStrategy';
export * from './OverrideFactory';
export * from './Override';
export { UpgradeResult, ValidationError };
/**
 * Check that the given manifest correctly describe overrides and that all
 * overrides are up to date
 */
export declare function validateManifest(opts: {
    manifestPath?: string;
    reactNativeVersion?: string;
}): Promise<ValidationError[]>;
/**
 * Return whether the override exists in the manifest
 */
export declare function hasOverride(overrideName: string, opts: {
    manifestPath?: string;
}): Promise<boolean>;
/**
 * Removes an override from the manifest if it exists.
 * @returns whether the override was removed
 */
export declare function removeOverride(overrideName: string, opts: {
    manifestPath?: string;
}): Promise<boolean>;
/**
 * Returns a factory to create overrides which may be added to the manifest
 */
export declare function getOverrideFactory(opts: {
    manifestPath?: string;
    reactNativeVersion?: string;
}): Promise<OverrideFactory>;
/**
 * Adds an override to the manifest
 */
export declare function addOverride(override: Override, opts: {
    manifestPath?: string;
}): Promise<void>;
/**
 * Receives notifications on progress during overide upgrades
 */
export declare type UpgradeProgressListener = (currentOverride: number, totalOverrides: number) => void;
/**
 * Attempts to automatically merge changes from the current version into
 * out-of-date overrides.
 */
export declare function upgradeOverrides(opts: {
    manifestPath?: string;
    reactNativeVersion?: string;
    allowConflicts: boolean;
    newVersion?: string;
    progressListener?: UpgradeProgressListener;
}): Promise<UpgradeResult[]>;
