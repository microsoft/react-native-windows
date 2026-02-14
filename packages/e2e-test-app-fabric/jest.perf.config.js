/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Jest configuration for performance tests.
 *
 * These tests run in a lightweight node environment using react-test-renderer
 * (NOT the full UIA automation environment used by E2E tests).
 *
 * Run: yarn perf
 * Update baselines: yarn perf:update
 *
 * @format
 * @ts-check
 */

// Ensure the rnx-kit preset picks up the windows platform
// so react-native resolves correctly and __DEV__ is defined.
process.env.RN_TARGET_PLATFORM = process.env.RN_TARGET_PLATFORM || 'windows';

const assetTransform = 'react-native-windows/jest/assetFileTransformer.js';

module.exports = {
  preset: '@rnx-kit/jest-preset',

  // Only run perf tests
  roots: ['<rootDir>/test/'],
  testMatch: ['**/__perf__/**/*.perf-test.{ts,tsx}'],

  // Perf tests need more time for multiple measurement runs
  testTimeout: 120000,

  // Run sequentially for consistent measurements
  maxWorkers: 1,

  // Setup file registers perf matchers
  setupFilesAfterEnv: ['./jest.perf.setup.ts'],

  // Transform configuration — matches existing E2E config
  transform: {
    '\\.(bmp|gif|jpg|jpeg|mp4|png|psd|svg|webp)$': assetTransform,
    'node_modules\\\\@?react-native\\\\.*': 'babel-jest',
    '@react-native-windows\\\\tester\\\\.*': 'babel-jest',
    '@react-native-windows\\\\perf-testing\\\\.*': 'babel-jest',
    'vnext\\\\.*': 'babel-jest',
    '\\.[jt]sx?$': 'babel-jest',
  },

  transformIgnorePatterns: ['jest-runner', 'node_modules\\\\safe-buffer'],

  globals: {
    __DEV__: true,
  },

  verbose: true,

  // In CI, also emit a machine-readable JSON results file.
  // The PerfJsonReporter collects all snapshot data after the run.
  reporters: process.env.CI
    ? [
        'default',
        [
          '@react-native-windows/perf-testing/lib-commonjs/ci/PerfJsonReporter',
          {outputFile: '.perf-results/results.json'},
        ],
      ]
    : ['default'],
};
