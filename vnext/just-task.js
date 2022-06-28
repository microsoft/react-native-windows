/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  task,
  series,
  condition,
  option,
  argv,
  tscTask,
} = require('just-scripts');

// Use the shared base configuration
require('@rnw-scripts/just-task');
require('@rnw-scripts/just-task/react-native-tasks');

const {execSync} = require('child_process');
const fs = require('fs');

option('production');
option('clean');

task('codegen', () => {
  execSync(
    'react-native-windows-codegen --files Libraries/**/*[Nn]ative*.js --namespace Microsoft::ReactNativeSpecs --libraryName rnwcore',
    {env: process.env},
  );
});

task('copyReadmeAndLicenseFromRoot', () => {
  fs.copyFileSync(
    path.resolve(__dirname, '../README.md'),
    path.resolve(__dirname, 'README.md'),
  );
  fs.copyFileSync(
    path.resolve(__dirname, '../LICENSE'),
    path.resolve(__dirname, 'LICENSE'),
  );
});

task('compileTsPlatformOverrides', tscTask());

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'copyRNLibraries',
    'copyReadmeAndLicenseFromRoot',
    'compileTsPlatformOverrides',
    'codegen',
  ),
);

task('clean', series('cleanRNLibraries'));

task('lint', series('eslint', 'flow-check'));
