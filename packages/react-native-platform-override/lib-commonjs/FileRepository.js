"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.bindVersion = void 0;
/**
 * Convert from a VersionedReactFileRepository to ReactFileRepository
 */
function bindVersion(repository, version) {
    return {
        listFiles: globs => repository.listFiles(globs, version),
        readFile: filename => repository.readFile(filename, version),
        stat: filename => repository.stat(filename, version),
        getVersion: () => version,
    };
}
exports.bindVersion = bindVersion;
//# sourceMappingURL=FileRepository.js.map