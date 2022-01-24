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
const mustache_1 = __importDefault(require("mustache"));
const path_1 = __importDefault(require("path"));
const semver_1 = __importDefault(require("semver"));
/**
 * Render PackageVersion.g.props
 */
async function renderPropsFile(version) {
    var _a;
    const templatePath = path_1.default.join(__dirname, '..', 'template', 'PackageVersion.g.props.hbs');
    const templateContent = await fs_1.default.readFile(templatePath);
    return mustache_1.default.render(templateContent.toString(), {
        version,
        major: semver_1.default.major(version),
        minor: semver_1.default.minor(version),
        patch: semver_1.default.patch(version),
        canary: ((_a = semver_1.default.prerelease(version)) === null || _a === void 0 ? void 0 : _a[0]) === 'canary',
    });
}
exports.default = renderPropsFile;
//# sourceMappingURL=renderPropsFile.js.map