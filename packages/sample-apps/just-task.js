/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task, series, parallel} = require('just-scripts');
const fs = require('fs');
const {execSync} = require('child_process');

// Use the shared base configuration
require('@rnw-scripts/just-task');

task('codegen', () => {
  execSync('npx --yes @react-native-community/cli codegen-windows --logging', {
    env: process.env,
  });
});

task('codegen:check', () => {
  execSync(
    'npx  --yes @react-native-community/cli codegen-windows --logging --check',
    {
      env: process.env,
    },
  );
});

task('prepareBundle', () => {
  fs.mkdirSync('windows/SampleAppCS/Bundle', {recursive: true});
  fs.mkdirSync('windows/SampleAppCPP/Bundle', {recursive: true});
});

task('build', series('codegen'));
