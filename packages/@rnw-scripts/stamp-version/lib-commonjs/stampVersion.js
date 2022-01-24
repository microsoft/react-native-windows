"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const fs_1 = __importDefault(require("@react-native-windows/fs"));
const path_1 = __importDefault(require("path"));
const yargs_1 = __importDefault(require("yargs"));
const semver_1 = __importDefault(require("semver"));
const package_utils_1 = require("@react-native-windows/package-utils");
const renderPropsFile_1 = __importDefault(require("./renderPropsFile"));
const { argv } = yargs_1.default
    .check((args) => {
    if (args._.length === 1 && semver_1.default.valid(args._[0])) {
        return true;
    }
    else {
        throw new Error('Usage: stamp-version <version>');
    }
})
    .showHelpOnFail(false);
(async () => {
    const version = argv._[0];
    await setPackageVersionProps(version);
})();
/**
 * Rewrites PackageVersion.g.props
 */
async function setPackageVersionProps(version) {
    const propsStr = await (0, renderPropsFile_1.default)(version);
    const rnwPackage = await (0, package_utils_1.findRepoPackage)('react-native-windows');
    const propsPath = path_1.default.join(rnwPackage.path, 'PropertySheets/Generated/PackageVersion.g.props');
    await fs_1.default.writeFile(propsPath, propsStr);
}
//# sourceMappingURL=stampVersion.js.map