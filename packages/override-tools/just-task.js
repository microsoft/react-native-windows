/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * 
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
  eslintTask,
  cleanTask,
} = require('just-scripts');

option('production');
option('clean');
option('ci');

task('eslint', () => {
  return eslintTask();
});
task('ts', () => {
  return tscTask({});
});
task('clean', () => {
  return cleanTask([path.join(process.cwd(), 'out')]);
});

task('build', series(condition('clean', () => argv().clean), 'ts'));

task('lint', series('eslint'));
