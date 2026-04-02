/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const assetTransform = 'react-native-windows/jest/assetFileTransformer.js';

module.exports = {
  preset: '@rnx-kit/jest-preset',

  roots: ['<rootDir>/test/'],
  testMatch: ['**/__native_perf__/**/*.native-perf-test.{ts,tsx}'],

  testEnvironment: '@react-native-windows/automation',

  testTimeout: 180000,

  maxWorkers: 1,

  setupFilesAfterEnv: [
    'react-native-windows/jest/setup',
    './jest.perf.setup.ts',
  ],

  testEnvironmentOptions: {
    app: 'RNTesterApp-Fabric',
    useRootSession: true,
    rootLaunchApp: true,
    enableAutomationChannel: true,
  },

  transform: {
    '\\.(bmp|gif|jpg|jpeg|mp4|png|psd|svg|webp)$': assetTransform,
    'node_modules\\\\@?react-native\\\\.*': 'babel-jest',
    '@react-native-windows\\\\tester\\\\.*': 'babel-jest',
    '@react-native-windows\\\\perf-testing\\\\.*': 'babel-jest',
    'vnext\\\\.*': 'babel-jest',
    '\\.[jt]sx?$': 'babel-jest',
  },

  transformIgnorePatterns: ['jest-runner', 'node_modules\\\\safe-buffer'],

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

  verbose: true,

  reporters: process.env.CI
    ? [
        'default',
        [
          '@react-native-windows/perf-testing/lib-commonjs/ci/PerfJsonReporter',
          {outputFile: '.native-perf-results/results.json'},
        ],
      ]
    : ['default'],
};
