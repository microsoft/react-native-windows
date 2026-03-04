"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Beachball change file detection and version bump invocation.
 *
 * @format
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.bumpVersions = exports.hasChangeFiles = void 0;
const fs_1 = __importDefault(require("fs"));
const path_1 = __importDefault(require("path"));
const proc_1 = require("./proc");
/**
 * Check whether there are pending beachball change files in the repo.
 *
 * Beachball stores change files as JSON in the `change/` directory at the
 * repo root. If there are any .json files there, there are pending changes.
 */
function hasChangeFiles(repoRoot) {
    const changeDir = path_1.default.join(repoRoot, 'change');
    if (!fs_1.default.existsSync(changeDir)) {
        return false;
    }
    const entries = fs_1.default.readdirSync(changeDir);
    return entries.some(entry => entry.endsWith('.json'));
}
exports.hasChangeFiles = hasChangeFiles;
/**
 * Run beachball bump to consume change files and update versions/changelogs.
 *
 * Invokes: npx beachball bump --branch <remote>/<target> --yes --verbose
 *
 * The --yes flag suppresses prompts.
 * The --verbose flag provides detailed output.
 * The --branch flag tells beachball the baseline branch to diff against.
 */
async function bumpVersions(opts) {
    await (0, proc_1.exec)(`npx beachball bump --branch ${opts.remote}/${opts.targetBranch} --yes --verbose`, { cwd: opts.cwd });
}
exports.bumpVersions = bumpVersions;
//# sourceMappingURL=beachballBump.js.map