"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Custom Jest reporter that writes perf results to a JSON file.
 * Used in CI to produce machine-readable output for downstream comparison
 * and PR comment generation.
 *
 * @format
 */
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.PerfJsonReporter = void 0;
const fs = __importStar(require("fs"));
const path = __importStar(require("path"));
const snapshotManager_1 = require("../matchers/snapshotManager");
/**
 * Custom Jest reporter that collects all perf snapshot data
 * after tests complete and writes a single JSON results file.
 *
 * Add to jest config:
 * ```js
 * reporters: [
 *   'default',
 *   ['@react-native-windows/perf-testing/lib-commonjs/ci/PerfJsonReporter', {
 *     outputFile: '.perf-results/results.json'
 *   }]
 * ]
 * ```
 */
class PerfJsonReporter {
    constructor(_globalConfig, options = {}) {
        this.outputFile = options.outputFile || '.perf-results/results.json';
    }
    /**
     * Called by Jest after all test suites have completed.
     */
    onRunComplete(_testContexts, results) {
        const suites = [];
        for (const suite of results.testResults) {
            // Load the snapshot file for this test suite (written by toMatchPerfSnapshot)
            const { file: snapshotFilePath } = snapshotManager_1.SnapshotManager.getSnapshotPath(suite.testFilePath);
            const snapshots = snapshotManager_1.SnapshotManager.load(snapshotFilePath);
            const passed = suite.testResults.filter(t => t.status === 'passed').length;
            const failed = suite.testResults.filter(t => t.status === 'failed').length;
            const totalDuration = suite.testResults.reduce((acc, t) => acc + (t.duration || 0), 0);
            suites.push({
                testFilePath: suite.testFilePath,
                suiteName: path.basename(suite.testFilePath, '.perf-test.tsx'),
                snapshots,
                passed,
                failed,
                totalDuration,
            });
        }
        const ciResults = {
            timestamp: new Date().toISOString(),
            branch: process.env.BUILD_SOURCEBRANCH ||
                process.env.GITHUB_HEAD_REF ||
                process.env.GITHUB_REF ||
                'unknown',
            commitSha: process.env.BUILD_SOURCEVERSION ||
                process.env.GITHUB_SHA ||
                'unknown',
            suites,
            summary: {
                totalSuites: results.numTotalTestSuites,
                totalTests: results.numTotalTests,
                passed: results.numPassedTests,
                failed: results.numFailedTests,
                durationMs: Date.now() - results.startTime,
            },
        };
        // Write results
        const outputDir = path.dirname(this.outputFile);
        if (!fs.existsSync(outputDir)) {
            fs.mkdirSync(outputDir, { recursive: true });
        }
        fs.writeFileSync(this.outputFile, JSON.stringify(ciResults, null, 2) + '\n', 'utf-8');
        console.log(`\n📊 Perf results written to: ${this.outputFile}`);
    }
}
exports.PerfJsonReporter = PerfJsonReporter;
// Default export for Jest reporter compatibility
module.exports = PerfJsonReporter;
//# sourceMappingURL=PerfJsonReporter.js.map