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
const graceful_fs_1 = __importDefault(require("graceful-fs"));
const util_1 = __importDefault(require("util"));
const importedMethods = [
    'access',
    'appendFile',
    'chmod',
    'chown',
    'copyFile',
    'lchmod',
    'lchown',
    // Not available in Node 12
    // 'lutimes',
    'link',
    'lstat',
    'mkdir',
    'mkdtemp',
    'open',
    'opendir',
    'readdir',
    'readFile',
    'readlink',
    'realpath',
    'rename',
    'rmdir',
    // Not available in Node 12
    // 'rm',
    'stat',
    'symlink',
    'truncate',
    'unlink',
    'utimes',
    'writeFile',
];
const asyncMethods = {
    ...promisifyImportedMethods(),
    exists: (path) => new Promise((resolve) => {
        graceful_fs_1.default.access(path, graceful_fs_1.default.constants.F_OK, (err) => err ? resolve(false) : resolve(true));
    }),
    readJsonFile: async (path, options) => {
        const opts = {
            encoding: (options === null || options === void 0 ? void 0 : options.encoding) || 'utf-8',
            flag: options === null || options === void 0 ? void 0 : options.flag,
        };
        return JSON.parse(await asyncMethods.readFile(path, opts));
    },
};
exports.default = asyncMethods;
/**
 * Import the patched callbacks from `graceful-fs` to promisify ourselves, since
 * `graceful-fs` does not patch `fs.promises`.
 */
function importMethods() {
    const copiedMethods = {};
    for (const methodName of importedMethods) {
        // @ts-ignore
        copiedMethods[methodName] = graceful_fs_1.default[methodName];
    }
    return copiedMethods;
}
function promisifyImportedMethods() {
    const promisifiedMethods = {};
    for (const [methodName, method] of Object.entries(importMethods())) {
        // @ts-ignore
        promisifiedMethods[methodName] = util_1.default.promisify(method);
    }
    return promisifiedMethods;
}
//# sourceMappingURL=asyncMethods.js.map