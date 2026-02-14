"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.DEFAULT_THRESHOLD = void 0;
/**
 * Default thresholds applied to all component tests unless overridden.
 *
 * - 10% regression threshold balances noise filtering vs real detection.
 * - 5 max render count is generous for most single-component scenarios.
 * - 10 runs provides statistical significance without being too slow.
 */
exports.DEFAULT_THRESHOLD = {
    maxDurationIncrease: 10,
    maxDuration: Infinity,
    maxRenderCount: 5,
    minRuns: 10,
};
//# sourceMappingURL=PerfThreshold.js.map