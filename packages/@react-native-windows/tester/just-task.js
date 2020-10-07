/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const path = require('path');

const {
  cleanTask,
  copyTask,
  eslintTask,
  series,
  task,
  tscTask,
} = require('just-scripts');

const rnTesterPath = path.dirname(
  require.resolve('@react-native/tester/package.json'),
);

task(
  'build',
  series(
    cleanTask(['js']),
    tscTask(),
    copyTask({paths: [path.join(rnTesterPath, 'js')], dest: 'js'}),
  ),
);

task('lint', series(eslintTask()));
task('lint:fix', series(eslintTask({fix: true})));
