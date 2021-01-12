"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.normalizePath = exports.unixPath = void 0;
const path = require("path");
const upath = require("upath");
/**
 * Convert a path to use Unix-style slashes
 */
function unixPath(filepath) {
    return upath.toUnix(filepath);
}
exports.unixPath = unixPath;
/**
 * Normalize a path and convert to use platform-specific slashes
 */
function normalizePath(filepath) {
    // path.normalize will convert unix paths to win32, but not the other way
    // around. Normalize to Unix paths first to get both cases.
    const unixStylePath = unixPath(filepath);
    return path.normalize(unixStylePath);
}
exports.normalizePath = normalizePath;
//# sourceMappingURL=PathUtils.js.map