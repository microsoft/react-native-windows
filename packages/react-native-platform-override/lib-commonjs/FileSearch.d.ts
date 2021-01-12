/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/**
 * Search for a manifest adjacent to the package above cwd
 */
export declare function findManifest(cwd?: string): Promise<string>;
/**
 * Seatch for the package.json for this package (react-native-platform-override)
 */
export declare function findThisPackage(): Promise<string>;
