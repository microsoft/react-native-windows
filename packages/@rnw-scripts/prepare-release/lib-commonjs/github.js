"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * GitHub operations via the gh CLI.
 *
 * @format
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.updatePR = exports.createPR = exports.findPR = void 0;
const fs_1 = __importDefault(require("fs"));
const os_1 = __importDefault(require("os"));
const path_1 = __importDefault(require("path"));
const proc_1 = require("./proc");
/**
 * Run a callback with a temporary file containing the given content.
 * The file is cleaned up after the callback completes (success or failure).
 */
async function withTempFile(content, fn) {
    const tmpFile = path_1.default.join(os_1.default.tmpdir(), `gh-pr-body-${process.pid}-${Date.now()}.md`);
    fs_1.default.writeFileSync(tmpFile, content, 'utf8');
    try {
        return await fn(tmpFile);
    }
    finally {
        try {
            fs_1.default.unlinkSync(tmpFile);
        }
        catch (_a) {
            // Ignore cleanup errors
        }
    }
}
/**
 * Find an existing open PR by head branch name.
 * Returns null if no matching PR exists.
 */
async function findPR(opts) {
    const repoFlag = opts.repo ? ` --repo "${opts.repo}"` : '';
    const result = await (0, proc_1.exec)(`gh pr list --head "${opts.head}" --json number,url --limit 1${repoFlag}`, { cwd: opts.cwd, fallback: '[]' });
    let prs;
    try {
        prs = JSON.parse(result);
    }
    catch (_a) {
        return null;
    }
    if (!Array.isArray(prs) || prs.length === 0) {
        return null;
    }
    return { number: prs[0].number, url: prs[0].url };
}
exports.findPR = findPR;
/**
 * Create a new pull request. Returns the PR number and URL.
 *
 * Uses --body-file with a temp file to avoid shell escaping issues
 * with multiline markdown content on Windows cmd.exe.
 */
async function createPR(opts) {
    return withTempFile(opts.body, async (bodyFile) => {
        const repoFlag = opts.repo ? ` --repo "${opts.repo}"` : '';
        const result = await (0, proc_1.exec)(`gh pr create --head "${opts.head}" --base "${opts.base}"` +
            ` --title "${escapeForShell(opts.title)}"` +
            ` --body-file "${bodyFile}"${repoFlag}`, { cwd: opts.cwd });
        // gh pr create outputs the PR URL on success
        const url = result.trim();
        const match = url.match(/\/pull\/(\d+)/);
        const number = match ? parseInt(match[1], 10) : 0;
        return { number, url };
    });
}
exports.createPR = createPR;
/**
 * Update an existing pull request's body text.
 *
 * Uses --body-file with a temp file to avoid shell escaping issues
 * with multiline markdown content on Windows cmd.exe.
 */
async function updatePR(opts) {
    await withTempFile(opts.body, async (bodyFile) => {
        const repoFlag = opts.repo ? ` --repo "${opts.repo}"` : '';
        await (0, proc_1.exec)(`gh pr edit ${opts.number} --body-file "${bodyFile}"${repoFlag}`, { cwd: opts.cwd });
    });
}
exports.updatePR = updatePR;
/**
 * Escape a string for safe inclusion in a double-quoted shell argument.
 * Used for single-line values like titles; multiline content uses --body-file.
 */
function escapeForShell(str) {
    return str
        .replace(/\\/g, '\\\\')
        .replace(/"/g, '\\"')
        .replace(/\$/g, '\\$')
        .replace(/`/g, '\\`');
}
//# sourceMappingURL=github.js.map