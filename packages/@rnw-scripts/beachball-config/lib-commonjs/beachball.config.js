"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const child_process_1 = require("child_process");
const package_utils_1 = require("@react-native-windows/package-utils");
const Options = {
    ...require("@rnw-scripts/generated-beachball-config"),
    // Do not generate tags for monorepo packages by default, to avoid a GitHub
    // release for every package.
    gitTags: false,
    hooks: {
        // Stamp versions when we publish a new package
        postbump: (_packagePath, name, version) => {
            if (name === 'react-native-windows') {
                console.log(`Stamping RNW Version ${version}`);
                (0, child_process_1.execSync)(`yarn stamp-version ${version}`);
            }
        }
    },
    transform: {
        changeFiles: (changeInfo) => Array.isArray(changeInfo.changes)
            ? { ...changeInfo, changes: changeInfo.changes.map(transformChangeInfo) }
            : transformChangeInfo(changeInfo),
    }
};
function transformChangeInfo(changeInfo) {
    return {
        ...changeInfo,
        type: correctChangeType(changeInfo),
        comment: formatComment(changeInfo.comment),
    };
}
function correctChangeType(changeInfo) {
    // Changes made to our main branch are often rolled into prerelease packages,
    // where a released branch should treat these changes as creating a new patch
    // release.
    if (changeInfo.type === 'prerelease' && !isPrerelease(changeInfo.packageName)) {
        return 'patch';
    }
    return changeInfo.type;
}
function isPrerelease(packageName) {
    var _a;
    const packageJson = (_a = (0, package_utils_1.findRepoPackageSync)(packageName)) === null || _a === void 0 ? void 0 : _a.json;
    return packageJson && packageJson.version.includes('-');
}
function formatComment(comment) {
    var _a, _b;
    // Remove versions from messages that look like "[0.xx] Message"
    return (_b = (_a = comment.match(/(\s*\[[\d\.]+\]\s*)?((.|\n)*)/)) === null || _a === void 0 ? void 0 : _a[2]) !== null && _b !== void 0 ? _b : comment;
}
module.exports = Options;
//# sourceMappingURL=beachball.config.js.map