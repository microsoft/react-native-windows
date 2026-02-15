/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';
import {SnapshotManager} from '../matchers/snapshotManager';
import type {SnapshotFile} from '../matchers/snapshotManager';

/**
 * Summary of a single test suite's perf results.
 */
export interface SuiteResult {
  testFilePath: string;
  suiteName: string;
  snapshots: SnapshotFile;
  passed: number;
  failed: number;
  totalDuration: number;
}

/**
 * Full CI perf run results.
 */
export interface CIRunResults {
  timestamp: string;
  branch: string;
  commitSha: string;
  suites: SuiteResult[];
  summary: {
    totalSuites: number;
    totalTests: number;
    passed: number;
    failed: number;
    durationMs: number;
  };
}

/**
 * Custom Jest reporter that collects perf snapshot data
 * and writes a JSON results file for CI.
 */
export class PerfJsonReporter {
  private outputFile: string;

  constructor(
    _globalConfig: Record<string, unknown>,
    options: {outputFile?: string} = {},
  ) {
    this.outputFile = options.outputFile || '.perf-results/results.json';
  }

  onRunComplete(
    _testContexts: Set<unknown>,
    results: {
      numTotalTestSuites: number;
      numPassedTestSuites: number;
      numFailedTestSuites: number;
      numTotalTests: number;
      numPassedTests: number;
      numFailedTests: number;
      startTime: number;
      testResults: Array<{
        testFilePath: string;
        testResults: Array<{
          ancestorTitles: string[];
          title: string;
          status: string;
          duration?: number;
        }>;
        perfTimer?: {start: number; end: number};
      }>;
    },
  ): void {
    const suites: SuiteResult[] = [];

    for (const suite of results.testResults) {
      // Load the snapshot file for this test suite (written by toMatchPerfSnapshot)
      const {file: snapshotFilePath} = SnapshotManager.getSnapshotPath(
        suite.testFilePath,
      );
      const snapshots = SnapshotManager.load(snapshotFilePath);

      const passed = suite.testResults.filter(
        t => t.status === 'passed',
      ).length;
      const failed = suite.testResults.filter(
        t => t.status === 'failed',
      ).length;
      const totalDuration = suite.testResults.reduce(
        (acc, t) => acc + (t.duration || 0),
        0,
      );

      suites.push({
        testFilePath: suite.testFilePath,
        suiteName: path.basename(suite.testFilePath, '.perf-test.tsx'),
        snapshots,
        passed,
        failed,
        totalDuration,
      });
    }

    const ciResults: CIRunResults = {
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
      fs.mkdirSync(outputDir, {recursive: true});
    }
    fs.writeFileSync(
      this.outputFile,
      JSON.stringify(ciResults, null, 2) + '\n',
      'utf-8',
    );

    console.log(`\n📊 Perf results written to: ${this.outputFile}`);
  }
}

// Default export for Jest reporter compatibility
module.exports = PerfJsonReporter;
