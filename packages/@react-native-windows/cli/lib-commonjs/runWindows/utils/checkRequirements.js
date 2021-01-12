"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.isWinSdkPresent = void 0;
const child_process_1 = require("child_process");
const path = require("path");
const shell = require("shelljs");
const version_1 = require("./version");
const REQUIRED_VERSIONS = {
    '10.0': {
        os: new version_1.default(6, 3),
        msbuild: new version_1.default(14, 0),
        visualstudio: new version_1.default(14, 0),
        windowssdk: new version_1.default(10, 0),
        phonesdk: new version_1.default(10, 0),
    },
};
function shortenVersion(version) {
    return /^(\d+(?:\.\d+)?)/.exec(version.toString())[1];
}
function getMinimalRequiredVersionFor(requirement, windowsTargetVersion) {
    return REQUIRED_VERSIONS[windowsTargetVersion][requirement];
}
function getInstalledWindowsSdks() {
    const installedSdks = [];
    const execString = 'reg query "HKLM\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows" /s /v InstallationFolder /reg:32';
    let output;
    try {
        output = child_process_1.execSync(execString).toString();
    }
    catch (e) {
        return installedSdks;
    }
    const re = /\\Microsoft SDKs\\Windows\\v(\d+\.\d+)\s*InstallationFolder\s+REG_SZ\s+(.*)/gim;
    let match;
    while ((match = re.exec(output))) {
        const sdkPath = match[2];
        if (shell.test('-e', path.join(sdkPath, 'SDKManifest.xml'))) {
            const sdkVersion = version_1.default.tryParse(match[1]);
            if (!sdkVersion) {
                throw new Error(`Unexpected SDK version format for '${match[1]}'`);
            }
            installedSdks.push(sdkVersion);
        }
    }
    return installedSdks;
}
function checkWinSdk(windowsTargetVersion) {
    const installedSdks = getInstalledWindowsSdks();
    const requiredVersion = getMinimalRequiredVersionFor('windowssdk', windowsTargetVersion);
    const hasSdkInstalled = installedSdks.some(installedSdk => installedSdk.eq(requiredVersion));
    if (hasSdkInstalled) {
        return shortenVersion(requiredVersion);
    }
    const shortenedVersion = shortenVersion(requiredVersion);
    throw new Error(`Windows SDK not found. Ensure that you have installed Windows ${shortenedVersion} SDK along with Visual Studio or install Windows ${shortenedVersion} SDK separately from https://dev.windows.com/en-us/downloads`);
}
function isWinSdkPresent(target) {
    return checkWinSdk(target);
}
exports.isWinSdkPresent = isWinSdkPresent;
//# sourceMappingURL=checkRequirements.js.map