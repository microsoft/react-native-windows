"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const globby = require("globby");
const path = require("path");
/**
 * Allows reading phsyical files based on a passed in directory
 */
class FileSystemRepository {
    constructor(baseDir) {
        this.baseDir = baseDir;
    }
    async listFiles(globs = ['**']) {
        return await globby(globs, {
            cwd: this.baseDir,
            dot: true,
        });
    }
    async readFile(filename) {
        const filePath = path.join(this.baseDir, filename);
        try {
            return await fs.promises.readFile(filePath);
        }
        catch (_a) {
            return null;
        }
    }
    async stat(filename) {
        const filePath = path.join(this.baseDir, filename);
        try {
            const stats = await fs.promises.stat(filePath);
            return stats.isDirectory() ? 'directory' : 'file';
        }
        catch (ex) {
            if (ex.code === 'ENOENT') {
                return 'none';
            }
            else {
                throw ex;
            }
        }
    }
    async writeFile(filename, content) {
        const filePath = path.join(this.baseDir, filename);
        await fs.promises.mkdir(path.dirname(filePath), { recursive: true });
        return fs.promises.writeFile(filePath, content);
    }
    async deleteFile(filename) {
        const filePath = path.join(this.baseDir, filename);
        await fs.promises.unlink(filePath);
    }
}
exports.default = FileSystemRepository;
//# sourceMappingURL=FileSystemRepository.js.map