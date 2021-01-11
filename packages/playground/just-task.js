/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const fs = require('fs');
const {task, series, eslintTask} = require('just-scripts');

function ensureDirectoryExists(filePath) {
  const dir = path.dirname(filePath);
  if (!fs.existsSync(filePath)) {
    ensureDirectoryExists(dir);
    fs.mkdirSync(filePath);
  }
}

task('eslint', () => {
  return eslintTask();
});
task('eslint:fix', () => {
  return eslintTask({fix: true});
});

task('lint', series('eslint'));
task('lint:fix', series('eslint:fix'));

task('prepareBundleWin32', () => {
  ensureDirectoryExists('windows/playground-win32/Bundle/Samples');
});
