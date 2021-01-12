"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.findThisPackage = exports.findManifest = void 0;
const fs = require("fs");
const path = require("path");
/**
 * Search for a manifest adjacent to the package above cwd
 */
async function findManifest(cwd) {
    const packagePath = await findFileAbove(cwd || process.cwd(), 'package.json');
    if (!packagePath) {
        throw new Error('This command must be run within a package');
    }
    const manifestPath = path.join(path.dirname(packagePath), 'overrides.json');
    if (!(await exists(manifestPath))) {
        throw new Error('Expected an "overrides.json" file at the root of the current package');
    }
    return manifestPath;
}
exports.findManifest = findManifest;
/**
 * Seatch for the package.json for this package (react-native-platform-override)
 */
async function findThisPackage() {
    const thisPackagePath = await findFileAbove(__dirname, 'package.json');
    if (!thisPackagePath) {
        throw new Error('Unable to find a package.json about our source code. Did directory structure change?');
    }
    return thisPackagePath;
}
exports.findThisPackage = findThisPackage;
/**
 * Search for the target path segment in all directories above the base
 */
async function findFileAbove(base, target) {
    const searchPath = path.resolve(base);
    const fullPath = path.join(searchPath, target);
    if (await exists(fullPath)) {
        return fullPath;
    }
    const searchPathParent = path.resolve(path.join(searchPath, '..'));
    if (searchPathParent === searchPath) {
        return null;
    }
    else {
        return findFileAbove(searchPathParent, target);
    }
}
/**
 * Asyncrhonusly tests if a file exists
 */
async function exists(filepath) {
    try {
        await fs.promises.access(filepath);
        return true;
    }
    catch (ex) {
        if (ex.code === 'ENOENT') {
            return false;
        }
        else {
            throw ex;
        }
    }
}
//# sourceMappingURL=FileSearch.js.map