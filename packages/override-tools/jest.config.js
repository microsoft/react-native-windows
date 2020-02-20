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
  roots: ['<rootDir>/src/'],

  // The test environment that will be used for testing
  testEnvironment: "node",

  // An array of regexp pattern strings that are matched against all source file paths, matched files will skip transformation
  transformIgnorePatterns: ['/node_modules/(?!io-ts/*|fp-ts/*)'],
};
