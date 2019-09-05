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

option('production');
option('clean');

task('eslint', () => {
  return eslintTask();
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

task('prepareBundle', () => {
  const file = 'windows/ReactUWPTestApp/Bundle';
  if (!fs.existsSync(file)) {
    fs.mkdirSync(file);
  }
});

task('build', series('eslint', 'ts'));
