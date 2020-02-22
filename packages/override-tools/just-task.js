/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {eslintTask, series, task, taskPresets} = require('just-scripts');

taskPresets.lib();

task('eslint', () => {
  return eslintTask();
});
task('lint', series('eslint'));
