"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.createPerfConfig = exports.DEFAULT_CONFIG = void 0;
/**
 * Default configuration for all perf tests.
 */
exports.DEFAULT_CONFIG = {
    defaultRuns: 10,
    defaultWarmupRuns: 1,
    defaultThreshold: {
        maxDurationIncrease: 10,
        maxDuration: Infinity,
        maxRenderCount: 5,
        minRuns: 10,
    },
    verbose: false,
    outputDir: '.perf-results',
};
/**
 * Create a custom config by merging overrides with defaults.
 */
function createPerfConfig(overrides) {
    var _a;
    return {
        ...exports.DEFAULT_CONFIG,
        ...overrides,
        defaultThreshold: {
            ...exports.DEFAULT_CONFIG.defaultThreshold,
            ...((_a = overrides.defaultThreshold) !== null && _a !== void 0 ? _a : {}),
        },
    };
}
exports.createPerfConfig = createPerfConfig;
//# sourceMappingURL=defaultConfig.js.map