/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {
  task,
  series,
  option,
  argv,
  tscTask,
  eslintTask,
} = require('just-scripts');
const fs = require('fs');
const path = require('path');

option('production');
option('clean');

task('eslint', () => {
  return eslintTask();
});
task('eslint:fix', () => {
  return eslintTask({fix: true});
});
task('ts', () => {
  return tscTask({
    pretty: true,
    ...(argv().production && {
      inlineSources: true,
    }),
    target: 'es6',
    module: 'commonjs',
  });
});

function ensureDirectoryExists(filePath) {
  const dir = path.dirname(filePath);
  if (!fs.existsSync(dir)) {
    ensureDirectoryExists(dir);
    fs.mkdirSync(dir);
  }
}

task('prepareBundle', () => {
  ensureDirectoryExists('windows/SampleAppCS/Bundle');
  ensureDirectoryExists('windows/SampleAppCPP/Bundle');
});

task('build', series('ts'));
task('lint', series('eslint'));
task('lint:fix', series('eslint:fix'));
