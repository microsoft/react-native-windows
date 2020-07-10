/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {jestTask, series, task} = require('just-scripts');

require('@rnw-scripts/just-task');

task('unitTest', jestTask({config: './jest.config.js', _: ['--verbose']}));
task(
  'endToEndTest',
  jestTask({config: './jest.e2e.config.js', runInBand: true, _: ['--verbose']}),
);

task('test', series('unitTest', 'endToEndTest'));
