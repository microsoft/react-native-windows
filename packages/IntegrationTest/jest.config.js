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
  // A list of paths to directories that Jest should use to search for files in
  roots: ['<rootDir>/runner/'],

  // The test environment that will be used for testing
  testEnvironment: 'node',

  // The pattern or patterns Jest uses to detect test files
  testRegex: '.*\\.test',

  // Default timeout of a test in milliseconds
  testTimeout: 300000,

  // A map from regular expressions to paths to transformers
  transform: {
    '^.+\\.[jt]sx?$': ['babel-jest', require('@rnw-scripts/babel-node-config')],
  },
};
