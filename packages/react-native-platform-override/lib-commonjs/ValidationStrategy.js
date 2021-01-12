"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.ValidationStrategies = void 0;
const Hash_1 = require("./Hash");
exports.ValidationStrategies = {
    /**
     * Validate that an override file exists
     */
    overrideFileExists: (overrideName) => ({
        validate: async (overrideRepo) => {
            switch (await overrideRepo.stat(overrideName)) {
                case 'file':
                    return [];
                case 'directory':
                    return [{ type: 'expectedFile', overrideName }];
                case 'none':
                    return [{ type: 'overrideNotFound', overrideName }];
            }
        },
    }),
    /**
     * Validate that a base file exists
     */
    baseFileExists: (overrideName, baseFile) => ({
        validate: async (_, reactRepo) => {
            switch (await reactRepo.stat(baseFile)) {
                case 'file':
                    return [];
                case 'directory':
                    return [{ type: 'expectedFile', overrideName }];
                case 'none':
                    return [{ type: 'baseNotFound', overrideName }];
            }
        },
    }),
    /**
     * Validate that an override directory exists
     */
    overrideDirectoryExists: (overrideName) => ({
        validate: async (overrideRepo) => {
            switch (await overrideRepo.stat(overrideName)) {
                case 'file':
                    return [{ type: 'expectedDirectory', overrideName }];
                case 'directory':
                    return [];
                case 'none':
                    return [{ type: 'overrideNotFound', overrideName }];
            }
        },
    }),
    /**
     * Validate that a base directory exists
     */
    baseDirectoryExists: (overrideName, baseDirectory) => ({
        validate: async (_, reactRepo) => {
            switch (await reactRepo.stat(baseDirectory)) {
                case 'file':
                    return [{ type: 'expectedDirectory', overrideName }];
                case 'directory':
                    return [];
                case 'none':
                    return [{ type: 'baseNotFound', overrideName }];
            }
        },
    }),
    /**
     * Validate that a base file/folder matches an expected hash if it exists
     */
    baseUpToDate: (overrideName, base, expectedBaseHash) => ({
        validate: async (_, reactRepo) => {
            const hash = await Hash_1.hashFileOrDirectory(base, reactRepo);
            if (!hash) {
                return [];
            }
            return hash === expectedBaseHash
                ? []
                : [{ type: 'outOfDate', overrideName }];
        },
    }),
    /**
     * Validate that an override meant to be a copy of a base file/folder has not
     * been tampered with
     */
    overrideCopyOfBase: (overrideName, base) => ({
        validate: async (overrideRepo, reactRepo) => {
            if ((await overrideRepo.stat(overrideName)) === 'none' ||
                (await reactRepo.stat(base)) === 'none') {
                return [];
            }
            const overrideHash = await Hash_1.hashFileOrDirectory(overrideName, overrideRepo);
            const baseHash = await Hash_1.hashFileOrDirectory(base, reactRepo);
            return overrideHash === baseHash
                ? []
                : [{ type: 'overrideDifferentFromBase', overrideName }];
        },
    }),
    /**
     * Validate that an override assumed to be different from its base is not
     * identical.
     */
    overrideDifferentFromBase: (overrideName, base) => ({
        validate: async (overrideRepo, reactRepo) => {
            if ((await overrideRepo.stat(overrideName)) === 'none' ||
                (await reactRepo.stat(base)) === 'none') {
                return [];
            }
            const overrideHash = await Hash_1.hashFileOrDirectory(overrideName, overrideRepo);
            const baseHash = await Hash_1.hashFileOrDirectory(base, reactRepo);
            return overrideHash === baseHash
                ? [{ type: 'overrideSameAsBase', overrideName }]
                : [];
        },
    }),
};
//# sourceMappingURL=ValidationStrategy.js.map