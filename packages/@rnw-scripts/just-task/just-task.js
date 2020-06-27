/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {cleanTask, eslintTask, jestTask, series, task, tscTask} = require('just-scripts');

task('clean', cleanTask(['lib-commonjs']));

task('ts:commonjs', tscTask({ module: 'commonjs', outDir: 'lib-commonjs' }));
task('ts', series('ts:commonjs'));

task('jest', jestTask());
task('jest:watch', jestTask({ watch: true }));

task('build', series('ts'));
task('test', series('jest'));

task('rebuild', series('clean', 'build'));

task('eslint', eslintTask());
task('eslint:fix', eslintTask({ fix: true }));

task('lint', series('eslint'));
task('lint:fix', series('eslint:fix'));
