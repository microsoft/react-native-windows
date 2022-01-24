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
const importedMethods = [
    'accessSync',
    'appendFileSync',
    'chmodSync',
    'chownSync',
    'closeSync',
    'copyFileSync',
    'createReadStream',
    'createWriteStream',
    'existsSync',
    'fchmodSync',
    'fchownSync',
    'fdatasyncSync',
    'fstatSync',
    'fsyncSync',
    'ftruncateSync',
    'futimesSync',
    'lchmodSync',
    'lchownSync',
    // Not available in Node 12
    // 'lutimesSync',
    'linkSync',
    'lstatSync',
    'mkdirSync',
    'mkdtempSync',
    'opendirSync',
    'openSync',
    'readdirSync',
    'readlinkSync',
    'readFileSync',
    'readSync',
    // Not available in Node 12
    // 'readvSync',
    'realpathSync',
    'realpathSync',
    'renameSync',
    'rmdirSync',
    // Not available in Node 12
    // 'rmSync',
    'statSync',
    'symlinkSync',
    'truncateSync',
    'unlinkSync',
    'utimesSync',
    'watch',
    'watchFile',
    'writeFileSync',
    'writeSync',
    'writeSync',
    'writevSync',
];
const syncMethods = {
    ...importMethods(),
    readJsonFileSync: (path, options) => {
        const opts = {
            encoding: (options === null || options === void 0 ? void 0 : options.encoding) || 'utf-8',
            flag: options === null || options === void 0 ? void 0 : options.flag,
        };
        return JSON.parse(syncMethods.readFileSync(path, opts));
    },
};
exports.default = syncMethods;
function importMethods() {
    const copiedMethods = {};
    for (const methodName of importedMethods) {
        // @ts-ignore
        copiedMethods[methodName] = graceful_fs_1.default[methodName];
    }
    return copiedMethods;
}
//# sourceMappingURL=syncMethods.js.map