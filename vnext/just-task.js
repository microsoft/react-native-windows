/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  cleanTask,
  task,
  series,
  condition,
  option,
  argv,
  tscTask,
  eslintTask,
} = require('just-scripts');
const {execSync} = require('child_process');
const fs = require('fs');
const copyRNLibaries = require('./Scripts/copyRNLibraries');

option('production');
option('clean');

task(
  'codegen',
  series(cleanTask({paths: ['./codegen']}), () => {
    execSync(
      'npx react-native-windows-codegen --files Libraries/**/Native*.js --namespace Microsoft::ReactNativeSpecs',
    );
  }),
);

task('flow-check', () => {
  require('child_process').execSync('npx flow check', {stdio: 'inherit'});
});

task('eslint', eslintTask());
task('eslint:fix', eslintTask({fix: true}));

task('copyRNLibraries', copyRNLibaries.copyTask(__dirname));

task('copyReadmeAndLicenseFromRoot', () => {
  fs.copyFileSync(
    path.resolve(__dirname, '../README.md'),
    path.resolve(__dirname, 'README.md'),
  );
  fs.copyFileSync(
    path.resolve(__dirname, '../LICENSE'),
    path.resolve(__dirname, 'LICENSE'),
  );
});

task(
  'compileTsPlatformOverrides',
  tscTask({
    pretty: true,
    ...(argv().production && {
      inlineSources: true,
    }),
    target: 'es5',
    module: 'commonjs',
  }),
);

task('cleanRnLibraries', copyRNLibaries.cleanTask(__dirname));

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'copyRNLibraries',
    'copyReadmeAndLicenseFromRoot',
    'compileTsPlatformOverrides',
    'codegen',
  ),
);

task('clean', series('cleanRnLibraries'));

task('lint', series('eslint', 'flow-check'));
task('lint:fix', series('eslint:fix'));
