"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const child_process_1 = require("child_process");
const find_repo_root_1 = require("@react-native-windows/find-repo-root");
/**
 * Run a command asyncronously, setting cwd to repo root if none is specified
 */
async function runCommand(command, opts = {}) {
    if (!opts.cwd) {
        opts.cwd = await find_repo_root_1.default();
    }
    return new Promise((resolve, reject) => {
        child_process_1.exec(command, opts, (err, stdout, stderr) => {
            if (err) {
                reject(err);
            }
            else {
                resolve({ stdout, stderr });
            }
        });
    });
}
exports.default = runCommand;
//# sourceMappingURL=runCommand.js.map