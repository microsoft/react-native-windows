"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.ThresholdPresets = void 0;
/**
 * Pre-configured threshold presets for different component categories.
 *
 * Use these as starting points when configuring component tests.
 * Override individual fields as needed.
 *
 * @example
 * ```typescript
 * import { ThresholdPresets } from '@react-native-windows/perf-testing';
 *
 * class MyListPerfTest extends ComponentPerfTestBase {
 *   get defaultThreshold() {
 *     return ThresholdPresets.list;
 *   }
 * }
 * ```
 */
exports.ThresholdPresets = {
    /**
     * For stable core components (View, Text, Image).
     * Tight thresholds — these should not regress.
     */
    core: {
        maxDurationIncrease: 10,
        maxDuration: Infinity,
        maxRenderCount: 2,
        minRuns: 10,
    },
    /**
     * For complex list/scroll components (FlatList, SectionList, ScrollView).
     * Slightly relaxed — these have more variability.
     */
    list: {
        maxDurationIncrease: 15,
        maxDuration: Infinity,
        maxRenderCount: 5,
        minRuns: 5,
    },
    /**
     * For interactive components with animations (RefreshControl, Touchable).
     * More relaxed to account for animation overhead.
     */
    interactive: {
        maxDurationIncrease: 20,
        maxDuration: Infinity,
        maxRenderCount: 10,
        minRuns: 10,
    },
    /**
     * For community/custom native components.
     * Most relaxed — gives new components room to stabilize.
     */
    community: {
        maxDurationIncrease: 25,
        maxDuration: Infinity,
        maxRenderCount: 15,
        minRuns: 5,
    },
};
//# sourceMappingURL=thresholdPresets.js.map