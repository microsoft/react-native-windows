/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/**
 * Try to find the currently installed React Native version by searching for and
 * reading it's package.json.
 */
export declare function getInstalledRNVersion(): Promise<string>;
/**
 * Return an object representing the package.json of our current package
 */
export declare function getNpmPackage(): Promise<any>;
