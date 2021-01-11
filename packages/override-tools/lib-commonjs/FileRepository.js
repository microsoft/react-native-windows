"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Convert from a VersionedReactFileRepository to ReactFileRepository
 */
function bindVersion(repository, version) {
    return {
        getFileContents: (filename) => repository.getFileContents(filename, version),
        getVersion: () => version,
    };
}
exports.bindVersion = bindVersion;
//# sourceMappingURL=FileRepository.js.map