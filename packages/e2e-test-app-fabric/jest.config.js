/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

// For a detailed explanation regarding each configuration property, visit:
// https://jestjs.io/docs/en/configuration.html

const assetTransform = 'react-native-windows/jest/assetFileTransformer.js';
const reactNativeTransform = './custom-transformer.js';
const defaultTransform = [
  'babel-jest',
  require('@rnw-scripts/babel-node-config'),
];

module.exports = {
  preset: '@rnx-kit/jest-preset',

  // A list of paths to directories that Jest should use to search for files in
  roots: ['<rootDir>/test/'],

  // The test environment that will be used for testing
  // This environment causes the app to launch and close after testing is complete.
  // Temporarily disabling due to breaks in UIA implementation.
  // disabled temporarily
  testEnvironment: '@react-native-windows/automation',

  // The pattern or patterns Jest uses to detect test files
  testRegex: ['.*\\.test\\.ts$', '.*\\.test\\.js$'],

  // Default timeout of a test in milliseconds
  testTimeout: 70000,

  // A map from regular expressions to paths to transformers
  transform: {
    '^.+\\.(bmp|gif|jpg|jpeg|mp4|png|psd|svg|webp)$': assetTransform,
    'node_modules\\\\@?react-native\\\\.*': reactNativeTransform,
    '@react-native-windows\\\\tester\\\\.*': reactNativeTransform,
    'vnext\\\\.*': reactNativeTransform,
    '^.+\\.[jt]sx?$': defaultTransform,
  },

  //snapshotResolver: 'react-native-windows/jest-snapshot-resolver.js',

  // An array of regexp pattern strings that are matched against all source file paths before transformation.
  // If the file path matches any of the patterns, it will not be transformed.
  transformIgnorePatterns: ['jest-runner', 'node_modules\\\\safe-buffer'],

  // Specifies the maximum number of workers the worker-pool will spawn for running tests.
  maxWorkers: 1,

  // Indicates whether each individual test should be reported during the run.
  verbose: true,

  // A list of paths to modules that run some code to configure or set up the testing framework
  // before each test file in the suite is executed
  setupFilesAfterEnv: ['react-native-windows/jest/setup', './jest.setup.js'],

  testEnvironmentOptions: {
    app: `windows\\Debug\\RNTesterApp-Fabric.exe`,
    enableAutomationChannel: true,
  },

  moduleFileExtensions: [
    'js',
    'windows.js',
    'android.js',
    'mjs',
    'cjs',
    'jsx',
    'ts',
    'windows.ts',
    'tsx',
    'windows.tsx',
    'json',
    'node',
  ],
};
