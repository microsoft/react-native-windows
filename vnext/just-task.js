/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const path = require('path');
const {
  task,
  copyTask,
  series,
  condition,
  option,
  argv,
  tscTask,
  eslintTask,
  apiExtractorVerifyTask,
  apiExtractorUpdateTask,
  cleanTask,
} = require('just-scripts');
const {execSync} = require('child_process');
const libPath = path.resolve(process.cwd(), 'lib');
const srcPath = path.resolve(process.cwd(), 'src');

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

task('eslint', () => {
  return eslintTask();
});
task('eslint:fix', () => {
  return eslintTask({fix: true});
});

task('copyFlowFiles', () => {
  return copyTask(['src/**/*.js'], '.');
});
task('initRNLibraries', () => {
  require('./Scripts/copyRNLibraries').copyRNLibraries(__dirname);
});

task('ts', () => {
  return tscTask({
    pretty: true,
    ...(argv().production && {
      inlineSources: true,
      sourceRoot: path.relative(libPath, srcPath),
    }),
    target: 'es5',
    module: 'commonjs',
  });
});
task('clean', () => {
  return cleanTask(
    ['dist', 'flow', 'jest', 'Libraries', 'RNTester'].map(p =>
      path.join(process.cwd(), p),
    ),
  );
});

task(
  'build',
  series(
    condition('clean', () => true || argv().clean),
    'initRNLibraries',
    'copyFlowFiles',
    'ts',
    'codegen',
    condition('apiExtractorVerify', () => argv().ci),
  ),
);

task('lint', series('eslint', 'flow-check'));
task('lint:fix', series('eslint:fix'));

task('api', series('apiExtractorUpdate', 'apiDocumenter'));
