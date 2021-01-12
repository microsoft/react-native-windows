"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.hashFileOrDirectory = exports.hashContent = exports.Hasher = void 0;
const crypto = require("crypto");
const path = require("path");
const isutf8_1 = require("isutf8");
/**
 * Creates a hash from content, attempting to normalize for line-feeds
 */
class Hasher {
    constructor() {
        this.hash = crypto.createHash('sha1');
    }
    feedContent(content) {
        if (typeof content === 'string' || isutf8_1.default(content)) {
            // Content is valid UTF8. Normalize line endings
            const normalizedStr = content
                .toString('utf8')
                .replace(/(?<!\r)\n/g, '\r\n');
            this.hash.update(normalizedStr);
        }
        else {
            // Content is binary. Hash the raw bytes
            this.hash.update(content);
        }
        return this;
    }
    digest() {
        return this.hash.digest('hex');
    }
}
exports.Hasher = Hasher;
/**
 * Convenience helper which hashes a single buffer
 */
function hashContent(content) {
    return new Hasher().feedContent(content).digest();
}
exports.hashContent = hashContent;
/**
 * Helper to hash a file/directory belonging to a repository
 */
async function hashFileOrDirectory(name, repo) {
    const type = await repo.stat(name);
    if (type === 'none') {
        return null;
    }
    if (type === 'file') {
        return hashContent((await repo.readFile(name)));
    }
    else {
        const hasher = new Hasher();
        for (const file of (await repo.listFiles([`${name}/**`])).sort()) {
            const contents = await repo.readFile(file);
            hasher.feedContent(contents);
            // Incorporate the filename to detect if renames happen
            hasher.feedContent(path.relative(name, file));
        }
        return hasher.digest();
    }
}
exports.hashFileOrDirectory = hashFileOrDirectory;
//# sourceMappingURL=Hash.js.map