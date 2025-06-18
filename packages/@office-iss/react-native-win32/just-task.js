/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const fs = require('fs');
const {
  task,
  series,
  condition,
  option,
  argv,
  tscTask,
} = require('just-scripts');

// Use the shared base configuration
require('@rnw-scripts/just-task');
require('@rnw-scripts/just-task/react-native-tasks');
require('@rnw-scripts/just-task/flow-tasks');

option('production');
option('clean');

task('ts', tscTask());

task('prepareBundle', () => {
  fs.mkdirSync(
    path.resolve(__dirname, 'dist/win32/dev/js/RNTesterApp.win32.bundle'),
    {recursive: true},
  );
});

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'copyRNLibraries',
    'ts',
  ),
);

task('clean', series('cleanRNLibraries'));

task('lint', series('prettier', 'eslint', 'flow-check'));
