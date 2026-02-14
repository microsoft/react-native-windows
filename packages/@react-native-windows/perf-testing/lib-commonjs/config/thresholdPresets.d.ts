/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import type { PerfThreshold } from '../interfaces/PerfThreshold';
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
export declare const ThresholdPresets: Readonly<Record<string, Required<PerfThreshold>>>;
