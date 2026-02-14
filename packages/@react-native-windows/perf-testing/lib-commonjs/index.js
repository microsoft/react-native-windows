"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @react-native-windows/perf-testing
 *
 * Performance testing utilities for React Native Windows components.
 *
 * This package provides:
 * - A measurement engine using React.Profiler
 * - A Jest matcher (`toMatchPerfSnapshot`) for baseline tracking
 * - An abstract base class for creating component perf tests
 * - Pre-built scenarios (mount, unmount, rerender)
 * - Threshold presets for different component categories
 * - Console and Markdown reporters for CI integration
 *
 * @example Quick Start (Community)
 * ```typescript
 * import {
 *   ComponentPerfTestBase,
 *   measurePerf,
 * } from '@react-native-windows/perf-testing';
 *
 * class MyComponentPerfTest extends ComponentPerfTestBase {
 *   readonly componentName = 'MyComponent';
 *   readonly category = 'custom';
 *   readonly testId = 'perf-test-my-component';
 *
 *   createComponent(props) {
 *     return <MyComponent testID={this.testId} {...props} />;
 *   }
 * }
 *
 * const test = new MyComponentPerfTest();
 * test.generateTestSuite();
 * ```
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.BaselineComparator = exports.PerfJsonReporter = exports.MarkdownReporter = exports.ConsoleReporter = exports.ThresholdPresets = exports.createPerfConfig = exports.DEFAULT_CONFIG = exports.SnapshotManager = exports.RerenderScenario = exports.UnmountScenario = exports.MountScenario = exports.ComponentPerfTestBase = exports.DEFAULT_THRESHOLD = exports.standardDeviation = exports.median = exports.mean = exports.PerfProfiler = exports.measurePerf = void 0;
// ─── Core Measurement ───
var measurePerf_1 = require("./core/measurePerf");
Object.defineProperty(exports, "measurePerf", { enumerable: true, get: function () { return measurePerf_1.measurePerf; } });
var PerfProfiler_1 = require("./core/PerfProfiler");
Object.defineProperty(exports, "PerfProfiler", { enumerable: true, get: function () { return PerfProfiler_1.PerfProfiler; } });
var statistics_1 = require("./core/statistics");
Object.defineProperty(exports, "mean", { enumerable: true, get: function () { return statistics_1.mean; } });
Object.defineProperty(exports, "median", { enumerable: true, get: function () { return statistics_1.median; } });
Object.defineProperty(exports, "standardDeviation", { enumerable: true, get: function () { return statistics_1.standardDeviation; } });
var PerfThreshold_1 = require("./interfaces/PerfThreshold");
Object.defineProperty(exports, "DEFAULT_THRESHOLD", { enumerable: true, get: function () { return PerfThreshold_1.DEFAULT_THRESHOLD; } });
// ─── Base Class ───
var ComponentPerfTestBase_1 = require("./base/ComponentPerfTestBase");
Object.defineProperty(exports, "ComponentPerfTestBase", { enumerable: true, get: function () { return ComponentPerfTestBase_1.ComponentPerfTestBase; } });
// ─── Built-in Scenarios ───
var MountScenario_1 = require("./scenarios/MountScenario");
Object.defineProperty(exports, "MountScenario", { enumerable: true, get: function () { return MountScenario_1.MountScenario; } });
var UnmountScenario_1 = require("./scenarios/UnmountScenario");
Object.defineProperty(exports, "UnmountScenario", { enumerable: true, get: function () { return UnmountScenario_1.UnmountScenario; } });
var RerenderScenario_1 = require("./scenarios/RerenderScenario");
Object.defineProperty(exports, "RerenderScenario", { enumerable: true, get: function () { return RerenderScenario_1.RerenderScenario; } });
// ─── Jest Matchers (auto-registers toMatchPerfSnapshot when imported) ───
require("./matchers");
var snapshotManager_1 = require("./matchers/snapshotManager");
Object.defineProperty(exports, "SnapshotManager", { enumerable: true, get: function () { return snapshotManager_1.SnapshotManager; } });
// ─── Configuration ───
var defaultConfig_1 = require("./config/defaultConfig");
Object.defineProperty(exports, "DEFAULT_CONFIG", { enumerable: true, get: function () { return defaultConfig_1.DEFAULT_CONFIG; } });
Object.defineProperty(exports, "createPerfConfig", { enumerable: true, get: function () { return defaultConfig_1.createPerfConfig; } });
var thresholdPresets_1 = require("./config/thresholdPresets");
Object.defineProperty(exports, "ThresholdPresets", { enumerable: true, get: function () { return thresholdPresets_1.ThresholdPresets; } });
// ─── Reporters ───
var ConsoleReporter_1 = require("./reporters/ConsoleReporter");
Object.defineProperty(exports, "ConsoleReporter", { enumerable: true, get: function () { return ConsoleReporter_1.ConsoleReporter; } });
var MarkdownReporter_1 = require("./reporters/MarkdownReporter");
Object.defineProperty(exports, "MarkdownReporter", { enumerable: true, get: function () { return MarkdownReporter_1.MarkdownReporter; } });
// ─── CI/CD Integration ───
var PerfJsonReporter_1 = require("./ci/PerfJsonReporter");
Object.defineProperty(exports, "PerfJsonReporter", { enumerable: true, get: function () { return PerfJsonReporter_1.PerfJsonReporter; } });
var BaselineComparator_1 = require("./ci/BaselineComparator");
Object.defineProperty(exports, "BaselineComparator", { enumerable: true, get: function () { return BaselineComparator_1.BaselineComparator; } });
//# sourceMappingURL=index.js.map