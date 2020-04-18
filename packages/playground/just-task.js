/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task, series, eslintTask, argv, tscTask} = require('just-scripts');

task('eslint', () => {
  return eslintTask();
});
task('eslint:fix', () => {
  return eslintTask({fix: true});
});

task('lint', series('eslint'));
task('lint:fix', series('eslint:fix'));

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

task('build', series('ts'));