/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task, series} = require('just-scripts');
const fs = require('fs');
const {execSync} = require('child_process');

// Use the shared base configuration
require('@rnw-scripts/just-task');

task('codegen', () => {
  execSync(
    'npx react-native-windows-codegen --file NativeMyModule.js --namespace SampleLibraryCpp',
  );
});

task('prepareBundle', () => {
  fs.mkdirSync('windows/SampleAppCS/Bundle', {recursive: true});
  fs.mkdirSync('windows/SampleAppCPP/Bundle', {recursive: true});
});

task('build', series('codegen'));
