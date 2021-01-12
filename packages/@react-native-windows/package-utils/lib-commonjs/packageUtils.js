"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.findRepoPackage = exports.findPackage = exports.enumerateRepoPackages = exports.WritableNpmPackage = exports.NpmPackage = void 0;
const _ = require("lodash");
const fs = require("fs");
const path = require("path");
const find_repo_root_1 = require("@react-native-windows/find-repo-root");
const getMonorepoPackages = require('get-monorepo-packages');
/**
 * Represents an NPM package
 */
class NpmPackage {
    constructor(pkgPath, pkgJson) {
        this.pkgPath = pkgPath;
        this.pkgJson = pkgJson;
    }
    /**
     * Create a NpmPackage from a given path to a package
     */
    static async fromPath(pkgPath) {
        return WritableNpmPackage.fromPath(pkgPath);
    }
    get path() {
        return this.pkgPath;
    }
    get json() {
        return _.cloneDeep(this.pkgJson);
    }
}
exports.NpmPackage = NpmPackage;
/**
 * Represents an NPM package with methods to modify its on-disk package.json
 */
class WritableNpmPackage extends NpmPackage {
    /**
     * Create a WritableNpmPackage from a given path to a package
     */
    static async fromPath(pkgPath) {
        const jsonPath = path.join(pkgPath, 'package.json');
        try {
            const jsonBuffer = await fs.promises.readFile(jsonPath);
            return new WritableNpmPackage(pkgPath, JSON.parse(jsonBuffer.toString()));
        }
        catch (ex) {
            if (ex.code === 'ENOENT') {
                return null;
            }
            throw ex;
        }
    }
    /**
     * Assign properties to the package. Uses lodash merge semantics to assign
     * properties (i.e. deep merge instead of shallow)
     *
     * @param props key/values to merge into the package.json
     */
    async mergeProps(props) {
        await this.setJson(_.merge(this.json, props));
    }
    /**
     * Assign properties to the package. Uses Object.assign semantics to assign
     * properties (i.e. shallow merge)
     *
     * @param props key/values to merge into the package.json
     */
    async assignProps(props) {
        await this.setJson(Object.assign(this.json, props));
    }
    /**
     * Set new JSON for the package from the given object
     */
    async setJson(jsonObj) {
        this.pkgJson = jsonObj;
        await fs.promises.writeFile(path.join(this.path, 'package.json'), JSON.stringify(this.json, null /*replacer*/, 2 /*space*/) + '\n');
    }
}
exports.WritableNpmPackage = WritableNpmPackage;
/**
 * Finds monorepo-local packages matching a given predicate. The root package
 * is not included.
 *
 * @param pred predicate describing whether to match a package
 */
async function enumerateRepoPackages(pred = async () => true) {
    const repoRoot = await find_repo_root_1.default();
    const allPackges = getMonorepoPackages(repoRoot).map(pkg => new WritableNpmPackage(pkg.location, pkg.package));
    const filteredPackages = [];
    for (const pkg of allPackges) {
        if (await pred(pkg)) {
            filteredPackages.push(pkg);
        }
    }
    return filteredPackages;
}
exports.enumerateRepoPackages = enumerateRepoPackages;
/**
 * Finds a package with a given name (local or dependency)
 */
async function findPackage(name, opts = {}) {
    const resolvePaths = require.resolve.paths(`${name}/package.json`);
    let pkgJsonPath;
    try {
        pkgJsonPath = require.resolve(`${name}/package.json`, {
            paths: [opts.searchPath || process.cwd(), ...resolvePaths],
        });
    }
    catch (ex) {
        if (ex.code === 'MODULE_NOT_FOUND') {
            return null;
        }
        else {
            throw ex;
        }
    }
    return new NpmPackage(path.dirname(pkgJsonPath), JSON.parse((await fs.promises.readFile(pkgJsonPath)).toString()));
}
exports.findPackage = findPackage;
/**
 * Finds a monorepo-local package with a given name
 */
async function findRepoPackage(name) {
    const packages = await enumerateRepoPackages(async (p) => p.json.name === name);
    if (packages.length === 0) {
        return null;
    }
    else {
        return packages[0];
    }
}
exports.findRepoPackage = findRepoPackage;
//# sourceMappingURL=packageUtils.js.map