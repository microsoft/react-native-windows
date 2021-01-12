"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.MockWritableFileRepository = exports.MockReactFileRepository = void 0;
const minimatch = require("minimatch");
const path = require("path");
class MockFileRepository {
    constructor(files, directories) {
        this.files = files.map(file => (Object.assign(Object.assign({}, file), { filename: path.normalize(file.filename) })));
        this.directories = (directories || []).map(dir => ({
            dirname: path.normalize(dir.dirname),
        }));
    }
    async listFiles(globs = ['**']) {
        const parsedGlobs = globs.map(g => new minimatch.Minimatch(g, { dot: true }));
        const includeGlobs = parsedGlobs.filter(m => !m.negate);
        const excludeGlobs = parsedGlobs.filter(m => m.negate);
        return this.files
            .map(file => file.filename)
            .filter(file => includeGlobs.some(g => g.match(file)))
            .filter(file => excludeGlobs.every(g => g.match(file)));
    }
    async readFile(filename) {
        const normalizedName = path.normalize(filename);
        const file = this.files.find(f => f.filename === normalizedName);
        return file ? Buffer.from(file.content) : null;
    }
    async stat(filename) {
        const normalizedName = path.normalize(filename);
        if (this.directories.find(dir => dir.dirname === normalizedName)) {
            return 'directory';
        }
        for (const file of this.files) {
            if (file.filename === normalizedName) {
                return 'file';
            }
            else if (file.filename.startsWith(normalizedName)) {
                return 'directory';
            }
        }
        return 'none';
    }
}
exports.default = MockFileRepository;
class MockReactFileRepository extends MockFileRepository {
    getVersion() {
        return '0.61.5';
    }
}
exports.MockReactFileRepository = MockReactFileRepository;
class MockWritableFileRepository extends MockFileRepository {
    async writeFile(filename, content) {
        const matchFile = this.files.find(file => file.filename === filename);
        if (matchFile) {
            matchFile.content = content;
        }
        else {
            this.files.push({ filename, content });
        }
    }
    async deleteFile(filename) {
        const matchIdx = this.files.findIndex(file => file.filename === filename);
        if (matchIdx === -1) {
            const err = new Error(`Mock file ${filename} not found`);
            err.code = 'ENOENT';
            throw err;
        }
        this.files.splice(matchIdx);
    }
}
exports.MockWritableFileRepository = MockWritableFileRepository;
//# sourceMappingURL=MockFileRepository.js.map