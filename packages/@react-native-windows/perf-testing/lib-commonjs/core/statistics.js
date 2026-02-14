"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.standardDeviation = exports.median = exports.mean = void 0;
/**
 * Calculate the arithmetic mean of an array of numbers.
 */
function mean(values) {
    if (values.length === 0) {
        return 0;
    }
    return values.reduce((sum, v) => sum + v, 0) / values.length;
}
exports.mean = mean;
/**
 * Calculate the median of an array of numbers.
 */
function median(values) {
    if (values.length === 0) {
        return 0;
    }
    const sorted = [...values].sort((a, b) => a - b);
    const mid = Math.floor(sorted.length / 2);
    return sorted.length % 2 !== 0
        ? sorted[mid]
        : (sorted[mid - 1] + sorted[mid]) / 2;
}
exports.median = median;
/**
 * Calculate the standard deviation of an array of numbers.
 */
function standardDeviation(values) {
    if (values.length < 2) {
        return 0;
    }
    const avg = mean(values);
    const squaredDiffs = values.map(v => (v - avg) ** 2);
    return Math.sqrt(squaredDiffs.reduce((sum, v) => sum + v, 0) / (values.length - 1));
}
exports.standardDeviation = standardDeviation;
//# sourceMappingURL=statistics.js.map