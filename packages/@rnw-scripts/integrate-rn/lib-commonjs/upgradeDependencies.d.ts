/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/**
 * Describes the dependencies of a package
 */
export declare type PackageDeps = {
    packageName: string;
    dependencies?: Record<string, string>;
    peerDependencies?: Record<string, string>;
    devDependencies?: Record<string, string>;
};
/**
 * Describes the dependencies of a monorepo-local NPM package (with extra
 * information)
 */
export declare type LocalPackageDeps = PackageDeps & {
    outOfTreePlatform: boolean;
};
/**
 * Describes a package before and after being updated
 */
export declare type PackageDiff = {
    oldPackage: PackageDeps;
    newPackage: PackageDeps;
};
/**
 * Upgrades all packages in the monorepo cwd resides in to use a new version
 * of react-native, updating non-react-native dependencies, devDependencies,
 * and peerDependencies where appropriate.
 */
export default function upgradeDependencies(newReactNativeVersion: string): Promise<void>;
/**
 * Given inputs on the packages of the monorepo along with old + new RN related
 * packages, determines the list of dependencies for every package in the
 * monorepo. Exported for testability. Assumes all dependencies and
 * devDependencies have already been updated to the newer version of
 * react-native.
 */
export declare function calcPackageDependencies(newReactNativeVersion: string, reactNativePackageDiff: PackageDiff, repoConfigPackageDiff: PackageDiff, localPackages: LocalPackageDeps[]): LocalPackageDeps[];
