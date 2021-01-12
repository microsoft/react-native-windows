"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.DiffStrategies = void 0;
exports.DiffStrategies = {
    /**
     * Assume the override is the same as the original
     */
    asssumeSame: () => ({
        diff: async () => '',
    }),
    /**
     * Assume the override is the same as the original
     */
    compareBaseFile: (overrideFile, baseFile, baseVersion) => ({
        diff: async (gitReactRepo, overrideRepo) => {
            const overrideContents = await overrideRepo.readFile(overrideFile);
            if (!overrideContents) {
                throw new Error(`Couldn't read override "${overrideFile}"`);
            }
            const patch = await gitReactRepo.generatePatch(baseFile, baseVersion, overrideContents);
            return patch
                .trim()
                .split('\n')
                .slice(4) // Ignore Git gunk
                .join('\n');
        },
    }),
};
//# sourceMappingURL=DiffStrategy.js.map