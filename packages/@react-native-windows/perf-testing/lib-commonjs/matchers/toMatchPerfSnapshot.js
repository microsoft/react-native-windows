"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const PerfThreshold_1 = require("../interfaces/PerfThreshold");
const snapshotManager_1 = require("./snapshotManager");
expect.extend({
    /**
     * Jest matcher that compares PerfMetrics against a stored baseline.
     *
     * On first run (or with `-u` flag), the baseline is written to a
     * `.perf.snap` file inside `__perf_snapshots__/`.
     *
     * On subsequent runs, it compares the new metrics against the baseline
     * and fails if any threshold is exceeded.
     *
     * @param received - The PerfMetrics from the current test run.
     * @param customThreshold - Optional per-test threshold overrides.
     *
     * @example
     * ```typescript
     * const perf = await viewPerfTest.measureMount();
     * expect(perf).toMatchPerfSnapshot();
     *
     * // With a relaxed threshold:
     * expect(perf).toMatchPerfSnapshot({ maxDurationIncrease: 20 });
     * ```
     */
    toMatchPerfSnapshot(received, customThreshold) {
        const testPath = expect.getState().testPath;
        const testName = expect.getState().currentTestName;
        // Resolve snapshot file location
        const { file: snapshotFile } = snapshotManager_1.SnapshotManager.getSnapshotPath(testPath);
        const snapshotKey = snapshotManager_1.SnapshotManager.buildKey(testName);
        const isUpdateMode = process.argv.includes('-u') ||
            process.argv.includes('--updateSnapshot');
        // Load existing snapshots
        const snapshots = snapshotManager_1.SnapshotManager.load(snapshotFile);
        const baseline = snapshots[snapshotKey];
        const threshold = { ...PerfThreshold_1.DEFAULT_THRESHOLD, ...customThreshold };
        // ─── UPDATE MODE or FIRST RUN: write new baseline ───
        if (isUpdateMode || !baseline) {
            snapshots[snapshotKey] = {
                metrics: received,
                threshold,
                capturedAt: new Date().toISOString(),
            };
            snapshotManager_1.SnapshotManager.save(snapshotFile, snapshots);
            return {
                pass: true,
                message: () => `✅ Perf snapshot ${baseline ? 'updated' : 'created'}: ${received.name}` +
                    ` (${received.meanDuration.toFixed(2)}ms, ${received.renderCount} renders)`,
            };
        }
        // ─── COMPARE MODE: check against baseline ───
        const errors = [];
        // Check duration regression (percentage)
        if (threshold.maxDurationIncrease !== undefined) {
            const percentChange = ((received.meanDuration - baseline.metrics.meanDuration) /
                baseline.metrics.meanDuration) *
                100;
            if (percentChange > threshold.maxDurationIncrease) {
                errors.push(`Duration regression: +${percentChange.toFixed(1)}% ` +
                    `(baseline: ${baseline.metrics.meanDuration.toFixed(2)}ms → ` +
                    `current: ${received.meanDuration.toFixed(2)}ms, ` +
                    `threshold: ${threshold.maxDurationIncrease}%)`);
            }
        }
        // Check absolute duration limit
        if (threshold.maxDuration !== undefined &&
            threshold.maxDuration !== Infinity &&
            received.meanDuration > threshold.maxDuration) {
            errors.push(`Duration exceeded: ${received.meanDuration.toFixed(2)}ms > ` +
                `${threshold.maxDuration}ms (absolute limit)`);
        }
        // Check render count
        if (threshold.maxRenderCount !== undefined &&
            received.renderCount > threshold.maxRenderCount) {
            errors.push(`Render count exceeded: ${received.renderCount} > ` +
                `${threshold.maxRenderCount}`);
        }
        if (errors.length > 0) {
            return {
                pass: false,
                message: () => `❌ Performance regression detected in "${received.name}":\n\n` +
                    errors.map(e => `  • ${e}`).join('\n') +
                    `\n\n📊 Baseline captured: ${baseline.capturedAt}` +
                    `\n💡 Run with -u flag to update baseline if intentional.`,
            };
        }
        // Calculate improvement for positive feedback
        const improvement = ((baseline.metrics.meanDuration - received.meanDuration) /
            baseline.metrics.meanDuration) *
            100;
        return {
            pass: true,
            message: () => `✅ "${received.name}" within threshold ` +
                `(${received.meanDuration.toFixed(2)}ms, ` +
                `${improvement > 0 ? `-${improvement.toFixed(1)}% faster` : 'no change'})`,
        };
    },
});
//# sourceMappingURL=toMatchPerfSnapshot.js.map