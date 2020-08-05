/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task, series, eslintTask} = require('just-scripts');
const fs = require('fs');
const path = require('path');
const {execSync} = require('child_process');

task('eslint', () => {
  return eslintTask();
});
task('eslint:fix', () => {
  return eslintTask({fix: true});
});

task('codegen', () => {
  execSync(
    'npx react-native-windows-codegen --file NativeMyModule.js --namespace SampleLibraryCpp',
  );
});

function ensureDirectoryExists(filePath) {
  const dir = path.dirname(filePath);
  if (!fs.existsSync(filePath)) {
    ensureDirectoryExists(dir);
    fs.mkdirSync(filePath);
  }
}

task('prepareBundle', () => {
  ensureDirectoryExists('windows/SampleAppCS/Bundle');
  ensureDirectoryExists('windows/SampleAppCPP/Bundle');
});

task('build', series('codegen'));
task('lint', series('eslint'));
task('lint:fix', series('eslint:fix'));
