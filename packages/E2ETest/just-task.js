/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {
  task,
  series,
  argv,
  tscTask,
  eslintTask,
  cleanTask,
} = require('just-scripts');
const fs = require('fs');

task('clean', cleanTask({ paths: ['dist'] }));
task('eslint', eslintTask());
task('eslint:fix', eslintTask({ fix: true }));
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

task('build', series('ts'));
task('lint', series('eslint'));
task('lint:fix', series('eslint:fix'));
