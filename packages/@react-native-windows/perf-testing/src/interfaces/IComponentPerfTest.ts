/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import type {PerfMetrics} from './PerfMetrics';
import type {PerfThreshold} from './PerfThreshold';

/**
 * Category for organizing perf tests.
 *
 * - 'core': Built-in RN components (View, Text, TextInput, Image, ScrollView)
 * - 'extended': Higher-level RN components (FlatList, SectionList, RefreshControl)
 * - 'custom': Community or app-specific components
 */
export type PerfTestCategory = 'core' | 'extended' | 'custom';

/**
 * Contract that all component performance tests must implement.
 */
export interface IComponentPerfTest {
  readonly componentName: string;
  readonly category: PerfTestCategory;
  readonly testId: string;
  readonly defaultThreshold: PerfThreshold;

  measureMount(): Promise<PerfMetrics>;
  measureUnmount(): Promise<PerfMetrics>;
  measureRerender(): Promise<PerfMetrics>;

  getCustomScenarios?(): IScenario[];
}

/**
 * A single named test scenario that produces performance metrics.
 */
export interface IScenario {
  name: string;
  description: string;
  run(): Promise<PerfMetrics>;
}
