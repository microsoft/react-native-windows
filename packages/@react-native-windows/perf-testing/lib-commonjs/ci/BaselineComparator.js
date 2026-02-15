"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Compares current perf results against stored baselines.
 * Used in CI to detect regressions and generate comparison reports.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.BaselineComparator = void 0;
const PerfThreshold_1 = require("../interfaces/PerfThreshold");
/**
 * Compares head (current) perf snapshots against base (main) snapshots.
 *
 * Produces ComparisonResult[] that can be fed into MarkdownReporter.
 */
class BaselineComparator {
    /**
     * Compare a single suite's head snapshot against its baseline.
     *
     * @param suiteName - Human-readable suite name (e.g., 'View')
     * @param headSnapshots - Snapshots from the current (PR) run
     * @param baseSnapshots - Snapshots from the baseline (main) branch
     * @param options - Comparison options
     */
    static compareSuite(suiteName, headSnapshots, baseSnapshots, options = {}) {
        var _a;
        const defaultThreshold = options.defaultThreshold || PerfThreshold_1.DEFAULT_THRESHOLD;
        const comparisons = [];
        // Compare all entries present in head
        for (const [key, headEntry] of Object.entries(headSnapshots)) {
            const baseEntry = baseSnapshots[key];
            const threshold = {
                ...defaultThreshold,
                ...(headEntry.threshold || {}),
                ...(((_a = options.thresholdOverrides) === null || _a === void 0 ? void 0 : _a[key]) || {}),
            };
            if (!baseEntry) {
                // New scenario — no baseline to compare
                comparisons.push({
                    metrics: headEntry.metrics,
                    baselineMetrics: undefined,
                    percentChange: undefined,
                    passed: true, // New scenarios always pass
                });
                continue;
            }
            const result = BaselineComparator.compareEntry(headEntry.metrics, baseEntry.metrics, threshold);
            comparisons.push(result);
        }
        // Flag scenarios removed in head (existed in base but not in head)
        for (const key of Object.keys(baseSnapshots)) {
            if (!headSnapshots[key]) {
                comparisons.push({
                    metrics: {
                        ...baseSnapshots[key].metrics,
                        name: `[REMOVED] ${baseSnapshots[key].metrics.name}`,
                    },
                    baselineMetrics: baseSnapshots[key].metrics,
                    percentChange: undefined,
                    passed: true, // Removal is not a regression
                });
            }
        }
        return {
            suiteName,
            comparisons,
            hasRegressions: comparisons.some(c => !c.passed),
        };
    }
    /**
     * Compare a single entry's metrics against its baseline.
     */
    static compareEntry(head, base, threshold) {
        // Merge with defaults so all fields are guaranteed defined
        const resolved = {
            ...PerfThreshold_1.DEFAULT_THRESHOLD,
            ...threshold,
        };
        // Use median for comparison — robust to outlier spikes
        const percentChange = base.medianDuration > 0
            ? ((head.medianDuration - base.medianDuration) / base.medianDuration) * 100
            : 0;
        const errors = [];
        // Check percentage increase AND absolute delta
        const absoluteDelta = head.medianDuration - base.medianDuration;
        if (percentChange > resolved.maxDurationIncrease &&
            absoluteDelta > resolved.minAbsoluteDelta) {
            errors.push(`Duration increased by ${percentChange.toFixed(1)}% / +${absoluteDelta.toFixed(2)}ms ` +
                `(threshold: ${resolved.maxDurationIncrease}% & ${resolved.minAbsoluteDelta}ms)`);
        }
        // Check absolute max
        if (head.medianDuration > resolved.maxDuration) {
            errors.push(`Duration ${head.medianDuration.toFixed(2)}ms exceeds ` +
                `max ${resolved.maxDuration}ms`);
        }
        // Check render count
        if (head.renderCount > resolved.maxRenderCount) {
            errors.push(`Render count ${head.renderCount} exceeds ` +
                `max ${resolved.maxRenderCount}`);
        }
        return {
            metrics: head,
            baselineMetrics: base,
            percentChange,
            passed: errors.length === 0,
            error: errors.length > 0 ? errors.join('; ') : undefined,
        };
    }
}
exports.BaselineComparator = BaselineComparator;
//# sourceMappingURL=BaselineComparator.js.map