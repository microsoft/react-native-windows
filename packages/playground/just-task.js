/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const fs = require('fs');
const {task, series, eslintTask} = require('just-scripts');

task('eslint', () => {
  return eslintTask();
});
task('eslint:fix', () => {
  return eslintTask({fix: true});
});

task('lint', series('eslint'));
task('lint:fix', series('eslint:fix'));

task('prepareBundleWin32', () => {
  const file = 'windows/playground-win32/Bundle';
  if (!fs.existsSync(file)) {
    fs.mkdirSync(file);
  }
});
