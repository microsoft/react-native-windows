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
require('@rnw-scripts/just-task/flow-tasks');

const {execSync} = require('child_process');
const fs = require('fs');

option('production');
option('clean');

function codegen(test) {
  execSync(
    `react-native-windows-codegen --files src/**/*Native*.js --namespace Microsoft::ReactNativeSpecs --libraryName rnwcore --modulesWindows --internalComponents --modulesCxx${
      test ? ' --test' : ''
    }`,
    {env: process.env},
  );
  execSync(
    `react-native-windows-codegen --files Microsoft.ReactNative.IntegrationTests/**/*Native*.js --namespace Microsoft::ReactNativeIntegrationTestSpecs --libraryName msrnIntegrationTests --modulesCxx --outputDirectory Microsoft.ReactNative.IntegrationTests/codegen${
      test ? ' --test' : ''
    }`,
    {env: process.env},
  );
}

task('codegen', () => codegen(false));
task('codegen:check', () => codegen(true));

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

task('lint', series('prettier', 'eslint', 'codegen:check', 'flow-check'));
