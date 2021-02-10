/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

// For a detailed explanation regarding each configuration property, visit:
// https://jestjs.io/docs/en/configuration.html

module.exports = {
  // The root directory that Jest should scan for tests and modules within
  rootDir: process.cwd(),

  // A list of paths to directories that Jest should use to search for files in
  roots: ['<rootDir>/src/'],

  // The test environment that will be used for testing
  testEnvironment: 'node',

  // The pattern or patterns Jest uses to detect test files
  testRegex: '/test/.*\\.test\.ts$',

  // Specifies the maximum number of workers the worker-pool will spawn for running tests.
  maxWorkers: 1,

  // Indicates whether each individual test should be reported during the run.
  verbose: true,

  // A map from regular expressions to paths to transformers
  transform: {
    '^.+\\.[jt]sx?$': ['babel-jest', require('@rnw-scripts/babel-node-config')],
  },
};
