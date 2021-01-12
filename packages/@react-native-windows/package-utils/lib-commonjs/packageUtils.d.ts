/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/**
 * Represents an NPM package
 */
export declare class NpmPackage {
    private readonly pkgPath;
    protected pkgJson: any;
    /**
     * Create a NpmPackage from a given path to a package
     */
    static fromPath(pkgPath: string): Promise<NpmPackage | null>;
    constructor(pkgPath: string, pkgJson: any);
    get path(): string;
    get json(): any;
}
/**
 * Represents an NPM package with methods to modify its on-disk package.json
 */
export declare class WritableNpmPackage extends NpmPackage {
    /**
     * Create a WritableNpmPackage from a given path to a package
     */
    static fromPath(pkgPath: string): Promise<WritableNpmPackage | null>;
    /**
     * Assign properties to the package. Uses lodash merge semantics to assign
     * properties (i.e. deep merge instead of shallow)
     *
     * @param props key/values to merge into the package.json
     */
    mergeProps(props: Record<string, any>): Promise<void>;
    /**
     * Assign properties to the package. Uses Object.assign semantics to assign
     * properties (i.e. shallow merge)
     *
     * @param props key/values to merge into the package.json
     */
    assignProps(props: Record<string, any>): Promise<void>;
    /**
     * Set new JSON for the package from the given object
     */
    setJson(jsonObj: any): Promise<void>;
}
/**
 * Finds monorepo-local packages matching a given predicate. The root package
 * is not included.
 *
 * @param pred predicate describing whether to match a package
 */
export declare function enumerateRepoPackages(pred?: (pkg: NpmPackage) => Promise<boolean>): Promise<WritableNpmPackage[]>;
/**
 * Finds a package with a given name (local or dependency)
 */
export declare function findPackage(name: string, opts?: {
    searchPath?: string;
}): Promise<NpmPackage | null>;
/**
 * Finds a monorepo-local package with a given name
 */
export declare function findRepoPackage(name: string): Promise<WritableNpmPackage | null>;
