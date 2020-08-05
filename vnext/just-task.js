/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  task,
  cleanTask,
  series,
  condition,
  option,
  argv,
  tscTask,
  tscWatchTask,
  eslintTask,
  apiExtractorVerifyTask,
  apiExtractorUpdateTask,
  parallel,
} = require('just-scripts');
const {execSync} = require('child_process');
const fs = require('fs');
const srcPath = path.resolve(process.cwd(), 'src');
const copyRNLibaries = require('./Scripts/copyRNLibraries');

option('production');
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

task('codegen', () => {
  execSync(
    'npx react-native-windows-codegen --files Libraries/**/Native*.js --namespace Microsoft::ReactNativeSpecs',
  );
});

task('flow-check', () => {
  require('child_process').execSync('npx flow check', {stdio: 'inherit'});
});

task('eslint', eslintTask());
task('eslint:fix', eslintTask({fix: true}));

task('copyRNLibraries', copyRNLibaries.copyTask(__dirname));

task('copyReadmeFromRoot', () => {
  fs.copyFileSync(
    path.resolve(__dirname, '../README.md'),
    path.resolve(__dirname, 'README.md'),
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
    'copyReadmeFromRoot',
    'compileTsPlatformOverrides',
    'codegen',
    condition('apiExtractorVerify', () => argv().ci),
  ),
);

task('clean', series('cleanRnLibraries'));

task('lint', series('eslint', 'flow-check'));
task('lint:fix', series('eslint:fix'));

task('api', series('apiExtractorUpdate', 'apiDocumenter'));
