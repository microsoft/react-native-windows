/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const copyRNLibaries = require('../../vnext/scripts/copyRNLibraries');
const path = require('path');
const fs = require('fs');
const {
  task,
  series,
  condition,
  option,
  argv,
  tscTask,
  eslintTask,
} = require('just-scripts');

option('production');
option('clean');

task('eslint', eslintTask());
task('eslint:fix', eslintTask({fix: true}));

task('copyRNLibraries', copyRNLibaries.copyTask(__dirname));

task('flow-check', () => {
  require('child_process').execSync('npx flow check', {stdio: 'inherit'});
});

task('ts', () => {
  return tscTask({
    pretty: true,
    ...(argv().production && {
      inlineSources: true,
    }),
    target: 'es5',
    module: 'commonjs',
  });
});

task('clean', copyRNLibaries.cleanTask(__dirname));

function ensureDirectoryExists(filePath) {
  const dir = path.dirname(filePath);
  if (!fs.existsSync(dir)) {
    ensureDirectoryExists(dir);
    fs.mkdirSync(dir);
  }
}

task('prepareBundle', () => {
  ensureDirectoryExists(
    path.resolve(__dirname, 'dist/win32/dev/js/RNTesterApp.win32.bundle'),
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

task('lint', series('eslint', 'flow-check'));
task('lint:fix', series('eslint:fix'));
