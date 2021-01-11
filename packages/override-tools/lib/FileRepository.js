/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
/**
 * Convert from a VersionedReactFileRepository to ReactFileRepository
 */
export function bindVersion(repository, version) {
    return {
        getFileContents: (filename) => repository.getFileContents(filename, version),
        getVersion: () => version,
    };
}
//# sourceMappingURL=FileRepository.js.map