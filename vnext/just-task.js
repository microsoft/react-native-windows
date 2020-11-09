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
  apiExtractorVerifyTask,
  apiExtractorUpdateTask,
} = require('just-scripts');
const {execSync} = require('child_process');
const fs = require('fs');
const copyRNLibaries = require('./Scripts/copyRNLibraries');

option('clean');
option('ci');

task('apiExtractorVerify', apiExtractorVerifyTask());
task('apiExtractorUpdate', apiExtractorUpdateTask());

task('apiDocumenter', () => {
  require('child_process').execSync(
    'npx @microsoft/api-documenter markdown -i temp -o ../docs/api',
    {stdio: 'inherit'},
  );
});

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

task('cleanRnLibraries', copyRNLibaries.cleanTask(__dirname));

task(
  'build',
  series(
    condition('clean', () => argv().clean),
    'copyRNLibraries',
    'copyReadmeAndLicenseFromRoot',
    'codegen',
    condition('apiExtractorVerify', () => argv().ci),
  ),
);

task('clean', series('cleanRnLibraries'));

task('lint', series('eslint', 'flow-check', tscTask()));
task('lint:fix', series('eslint:fix'));

task('api', series('apiExtractorUpdate', 'apiDocumenter'));
