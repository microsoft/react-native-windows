/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

export {measurePerf} from './core/measurePerf';
export type {MeasurePerfOptions, RenderHelpers} from './core/measurePerf';
export {PerfProfiler} from './core/PerfProfiler';
export type {ProfilerResult} from './core/PerfProfiler';
export {
  mean,
  median,
  standardDeviation,
  coefficientOfVariation,
  mannWhitneyU,
} from './core/statistics';
export type {MannWhitneyResult} from './core/statistics';

export type {PerfMetrics} from './interfaces/PerfMetrics';
export type {PerfThreshold} from './interfaces/PerfThreshold';
export {DEFAULT_THRESHOLD} from './interfaces/PerfThreshold';
export type {
  IComponentPerfTest,
  IScenario,
  PerfTestCategory,
} from './interfaces/IComponentPerfTest';

export {ComponentPerfTestBase} from './base/ComponentPerfTestBase';

export {MountScenario} from './scenarios/MountScenario';
export {UnmountScenario} from './scenarios/UnmountScenario';
export {RerenderScenario} from './scenarios/RerenderScenario';

import './matchers';
export {SnapshotManager} from './matchers/snapshotManager';
export type {SnapshotEntry, SnapshotFile} from './matchers/snapshotManager';

export {DEFAULT_CONFIG, createPerfConfig} from './config/defaultConfig';
export type {PerfTestConfig} from './config/defaultConfig';
export {ThresholdPresets} from './config/thresholdPresets';

export {ConsoleReporter} from './reporters/ConsoleReporter';
export {MarkdownReporter} from './reporters/MarkdownReporter';
export type {ComparisonResult} from './reporters/MarkdownReporter';

export {PerfJsonReporter} from './ci/PerfJsonReporter';
export type {SuiteResult, CIRunResults} from './ci/PerfJsonReporter';
export {BaselineComparator} from './ci/BaselineComparator';
export type {CompareOptions, SuiteComparison} from './ci/BaselineComparator';
