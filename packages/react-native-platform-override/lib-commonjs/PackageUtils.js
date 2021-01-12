"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.getNpmPackage = exports.getInstalledRNVersion = void 0;
const fs = require("fs");
const FileSearch_1 = require("./FileSearch");
let npmPackage = null;
/**
 * Try to find the currently installed React Native version by searching for and
 * reading it's package.json.
 */
async function getInstalledRNVersion() {
    const rnPackage = require('react-native/package.json');
    const version = rnPackage.version;
    if (typeof version !== 'string') {
        throw new Error('Unexpected formt of React Native package.json');
    }
    return version;
}
exports.getInstalledRNVersion = getInstalledRNVersion;
/**
 * Return an object representing the package.json of our current package
 */
async function getNpmPackage() {
    if (npmPackage !== null) {
        return npmPackage;
    }
    const npmPackagePath = await FileSearch_1.findThisPackage();
    const npmPackageContent = await fs.promises.readFile(npmPackagePath);
    npmPackage = JSON.parse(npmPackageContent.toString());
    return npmPackage;
}
exports.getNpmPackage = getNpmPackage;
//# sourceMappingURL=PackageUtils.js.map